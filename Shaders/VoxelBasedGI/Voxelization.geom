#version 430 core
#extension GL_ARB_shading_language_include : require
layout(triangles)in;
layout(triangle_strip, max_vertices = 3)out;

#include "/ConeTracingGlobals.glsl"

in VertexOut
{
	vec4 WorldPos;
	vec3 Color;
	vec3 Normal;
	mat3 TBN;
	vec2 TexCoords;
}IN[];

out GeomOut
{
	vec3 WorldPos;
	vec3 Color;
	vec3 Normal;
	mat3 TBN;
	vec2 TexCoords;
}OUT;

layout(std140,binding=10)uniform
Scenedata
{
	mat4 projMat;
	mat4 viewMat;
	float totalTime;
	float deltaTime;
}SceneD;
void main()
{
	vec3 triangleNormal=normalize(cross(IN[1].WorldPos.xyz - IN[0].WorldPos.xyz,
		IN[2].WorldPos.xyz - IN[0].WorldPos.xyz));
	float xVal=abs(triangleNormal.x);
	float yVal=abs(triangleNormal.y);
	float zVal=abs(triangleNormal.z);

	vec3 worldPos1 = IN[0].WorldPos.xyz;
	vec3 worldPos2 = IN[1].WorldPos.xyz;
	vec3 worldPos3 = IN[2].WorldPos.xyz;

	// Swizzling instead of a rotation matrix since it's faster and easier
	if(xVal>yVal&&xVal>zVal)
	{
		worldPos1 =	worldPos1.zyx;
		worldPos2 =	worldPos2.zyx;
		worldPos3 =	worldPos3.zyx;
	}
	else if(yVal>xVal&&yVal>zVal)
	{
		worldPos1 =	worldPos1.xzy;
		worldPos2 = worldPos2.xzy;
		worldPos3 = worldPos3.xzy;
	}
	// No rotation needed for z-Axis

	// Scale Voxels based on the desired voxelization area
	worldPos1 *= VoxelUniforms.VoxelsPerUnit;
	worldPos2 *= VoxelUniforms.VoxelsPerUnit;
	worldPos3 *= VoxelUniforms.VoxelsPerUnit;
	
	// z = 1 due to orthogonal projection
	worldPos1.z = 1;
	worldPos2.z = 1;
	worldPos3.z = 1;

	gl_Position=VoxelUniforms.orthoMat * vec4(worldPos1, 1);
	OUT.WorldPos = IN[0].WorldPos.xyz;
	OUT.Normal = IN[0].Normal;
	OUT.TexCoords = IN[0].TexCoords;
	OUT.TBN = IN[0].TBN;
	EmitVertex();

	gl_Position= VoxelUniforms.orthoMat * vec4(worldPos2, 1);
	OUT.WorldPos = IN[1].WorldPos.xyz;
	OUT.Normal = IN[1].Normal;
	OUT.TexCoords = IN[1].TexCoords;
	OUT.TBN = IN[1].TBN;
	EmitVertex();

	gl_Position= VoxelUniforms.orthoMat * vec4(worldPos3, 1);
	OUT.WorldPos = IN[2].WorldPos.xyz;
	OUT.Normal = IN[2].Normal;
	OUT.TexCoords = IN[2].TexCoords;
	OUT.TBN = IN[2].TBN;
	EmitVertex();
	EndPrimitive();
}
