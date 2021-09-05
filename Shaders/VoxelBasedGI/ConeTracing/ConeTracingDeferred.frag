#version 440 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_bindless_texture:enable
#extension GL_NV_gpu_shader5:enable


uniform sampler3D colorTex3DFirstBounce;
#include "/Globals.glsl"
#include "/ConeTracingGlobals.glsl"
#include "/ConeTracingFunctions.glsl"
#include "/PBRFunctions.glsl"

out vec4 FragColor;

in VertexOut
{
	vec2 TexCoords;
}IN;

uniform Material material;
uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D emissiveBuffer;
uniform sampler2D diffRoughBuffer;
uniform samplerCube skybox;

void main()
{
	vec4 posVals=texture(positionBuffer,IN.TexCoords);
	vec4 worldPos=vec4(posVals.xyz,1);
	vec3 normal=texture(normalBuffer,IN.TexCoords).rgb;
	vec4 diffSpec=texture(diffRoughBuffer,IN.TexCoords);
	vec3 emission = texture(emissiveBuffer, IN.TexCoords).xyz;

	vec3 diffuseCol=diffSpec.xyz;
	// Use material roughness or custom set roughness 
	float roughness= ConeTraceData.lightingSettings.w == 1 ? ConeTraceData.lightingSettings.y : diffSpec.a;
	
	float voxelSize = VoxelUniforms.VoxelSize / 0.5f;

	vec3 viewDir=normalize(Lights.cameraPos-worldPos.xyz);

	vec4 diffGI = vec4(0);
	// Useful precomputation
	float tanConeHalfAngle = tan(ConeTraceData.showGI.w * 0.5f);

	// Normal not in cone direction array
	diffGI += coneWeights[0] * traceDiffuseCone(colorTex3DFirstBounce, worldPos.xyz, normal, normal, 
		voxelSize, tanConeHalfAngle);
	for(int i = 1; i < 6; ++i)
	{
		vec3 sphereVec = ConeTraceData.randomUnitPoints[i].xyz;
		diffGI += coneWeights[i] * traceDiffuseCone(colorTex3DFirstBounce, worldPos.xyz, normal, sphereVec, 
		voxelSize, tanConeHalfAngle);
	}

	vec3 rslt = vec3(0);
	if(ConeTraceData.showGI.x > 0)
		rslt+=computeAmbient(diffuseCol * diffGI.a);
	else
		rslt+=computeAmbient(diffuseCol);

	vec3 lighting=vec3(0);

	// Direct Light loop
	for(int i=0;i<MAX_LIGHTS;++i)
	{
		PointLight light=Lights.lights[i];
		vec3 lightPos=light.pos.xyz;
		vec3 lightDir = normalize(lightPos - worldPos.xyz);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		vec3 l = computePointLight(worldPos.xyz, lightPos, viewDir, normal, halfwayDir, light.color.xyz, diffuseCol, roughness);

		if(ConeTraceData.showGI.x > 0)
			lighting += (l * traceShadowCone(colorTex3DFirstBounce, worldPos.xyz, normal, lightDir,  tan(0.04f * 0.5f), voxelSize, length(worldPos.xyz - lightPos)));
		else
			lighting += l;
	}
	if(ConeTraceData.showGI.x > 0)
	{
		rslt += (diffGI.xyz * diffGI.a);
		rslt += traceSpecularCone(colorTex3DFirstBounce, worldPos.xyz, normalize(reflect(-viewDir, normal)) * 1.5, voxelSize, ConeTraceData.specularSettings.y, roughness); // without 1.5 the specular is too restricted
	}
	rslt+=lighting;
	rslt.xyz += emission;
	
	// Choose tonemapping operator
	if(ConeTraceData.lightingSettings.z == 0)
		rslt = ModifiedReinhardTMO(rslt);
	else if(ConeTraceData.lightingSettings.z == 1)
		rslt = AcesTMO(rslt);
	else if(ConeTraceData.lightingSettings.z == 2)
		rslt = Uncharted2TMO(rslt);

	// Skybox writes 1 into w value and doesn't need shading
	if(posVals.w > 0.9)
		rslt = diffuseCol;

	// Gamma correction
	rslt = pow(rslt, vec3(1.0)/vec3(2.2));

	FragColor=vec4(rslt ,1);
}