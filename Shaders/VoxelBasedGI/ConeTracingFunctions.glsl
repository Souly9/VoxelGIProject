
vec4 sampleVoxels(sampler3D tex, float mip, vec3 worldPos)
{
	vec3 uv = worldPos / (VoxelUniforms.VoxelDimensions / VoxelUniforms.VoxelsPerUnit);
	return textureLod(tex, uv, mip);
}

vec4 traceDiffuseCone(sampler3D colorTex, vec3 worldPos, vec3 normal, vec3 pointOnSphere, float voxelSize, float tanHalfConeAngle)
{
	// Bias direction based on normal
	vec3 coneVec = reflect(normal, pointOnSphere);
	if(dot(normal, coneVec) < 0)
		coneVec = -coneVec;
	coneVec = normalize(coneVec);
	float dist = voxelSize * 1.01;
	vec3 startPos = worldPos + coneVec * dist;

	vec3 rslt = vec3(0);
	float alpha = 0;

	// All textures are VoxelDimensions^3, formula given in OpenGL 4.6 specification 8.14.3
	float maxMIP =  1.0 + floor(log2(VoxelUniforms.VoxelDimensions));

	while(dist < ConeTraceData.showGI.z && alpha < 1)
	{
		vec3 worldConePos = startPos + (coneVec * dist);
		float coneDiameter = 2.0f * dist * tanHalfConeAngle;

		float mip = log2(coneDiameter * VoxelUniforms.VoxelDimensions);

		vec4 color = sampleVoxels(colorTex, min(mip, maxMIP), worldConePos); 

		// Front-to-back accumulation
		float a = 1.0f - alpha;
		rslt.rgb += a * color.rgb;
		alpha += a * color.a;

		dist += max(coneDiameter * ConeTraceData.showGI.y, voxelSize * 1.01); // So the step size doesn't become smaller than voxel size
	}

	return vec4(rslt, alpha); 
}

float traceShadowCone(sampler3D colorTex, vec3 worldPos, vec3 normal, vec3 dir, float voxelSize, float tanHalfConeAngle, float maxDist)
{
	vec3 coneVec = dir;
	float dist = voxelSize * 1.01;
	vec3 startPos = worldPos + coneVec * dist;

	float rslt = 0;

	// All textures are VoxelDimensions^3, formula given in OpenGL 4.6 specification 8.14.3
	float maxMIP =  1.0 + floor(log2(VoxelUniforms.VoxelDimensions));

	while(dist < maxDist && rslt < 1)
	{
		vec3 worldConePos = startPos + (coneVec * dist);
		float coneDiameter = 2.0f * dist * tanHalfConeAngle;

		// Classic approach leads to too hard falloff
		float squared = pow(dist, 2);
		// One near sample for sharpness, one far sample for softness
		// Values adjusted by eye
		float nearSample =1.0 + (0.75 * squared);
		float farSample = 4.0 + (0.8 * squared);

		vec4 color = sampleVoxels(colorTex, min(nearSample, maxMIP), worldConePos); 
		vec4 color2 = sampleVoxels(colorTex, min(farSample, maxMIP), worldConePos); 

		float finalA = (ConeTraceData.shadowSettings.z * color.a) + (ConeTraceData.shadowSettings.w * color2.a);
		
		rslt += (1.0f - rslt) * finalA;
		dist += max(coneDiameter * 0.001, voxelSize * 1.01);
	}
	return 1.0f - min(rslt, 1.0f); 
}

vec3 traceSpecularCone(sampler3D colorTex, vec3 worldPos, vec3 dir, float voxelSize, float coneAngle, float roughness)
{
	vec3 coneVec = dir;
	float dist = voxelSize * 1.01;
	vec3 startPos = worldPos + (coneVec * dist);

	// Specular Cone Angle should approach Diffuse Cone Angle with increasing Roughness
	coneAngle = mix(coneAngle, ConeTraceData.showGI.w, roughness);

	float tanHalfConeAngle = tan(coneAngle * 0.5f);

	vec3 rslt = vec3(0);
	float alpha = 0;
	
	// All textures are VoxelDimensions^3, formula given in OpenGL 4.6 specification 8.14.3
	float maxMIP =  1.0 + floor(log2(VoxelUniforms.VoxelDimensions));

	while(dist < ConeTraceData.specularSettings.w && alpha < 1)
	{
		vec3 worldConePos = startPos + (coneVec * dist);
		float coneDiameter = 2.0f * dist * tanHalfConeAngle;

		float mip = log2(coneDiameter * VoxelUniforms.VoxelDimensions);
		// Looks slightly better
		mip *= 0.9;

		vec4 color = sampleVoxels(colorTex, min(mip, maxMIP), worldConePos);

		// Front-to-back accumulation
		float a = 1.0f - alpha;
		rslt.rgb += a * color.rgb;
		alpha += a * color.a;

		dist += max(coneDiameter * ConeTraceData.specularSettings.z, voxelSize * 1.01);
	}
	return rslt;
}
