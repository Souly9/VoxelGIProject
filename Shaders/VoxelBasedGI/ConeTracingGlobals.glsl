layout(std140,binding=12)uniform VoxelData
{
	mat4 orthoMat;
	float VoxelsPerUnit;
	uint VoxelDimensions;
	float VoxelSize;
}VoxelUniforms;

layout(std140, binding = 50) uniform 
ConeTraceUniforms
{
	vec4 randomUnitPoints[12];
	vec4 showGI;
	// Check GUI Class
	vec4 shadowSettings;
	vec4 specularSettings;
	vec4 lightingSettings;
} ConeTraceData;

float coneWeights[6] = float[6]
(
		0.45, 0.15, 0.15, 0.15, 0.15, 0.15
);

ivec3 WorldToVoxel(vec3 worldPos)
{
	vec3 voxelSpace = worldPos * VoxelUniforms.VoxelsPerUnit;
	return ivec3(floor(voxelSpace));
}
vec3 VoxelToWorld(ivec3 voxelSpace)
{
	return vec3(voxelSpace) / (VoxelUniforms.VoxelsPerUnit);
}

uint flattenPos(ivec3 pos)
{
	return pos.x+pos.y*VoxelUniforms.VoxelDimensions+
	pos.z*VoxelUniforms.VoxelDimensions*VoxelUniforms.VoxelDimensions;
}