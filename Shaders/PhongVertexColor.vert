#version 440 
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiTangent;
layout(location = 5) in vec3 vColor;

layout(std140, binding = 10) uniform 
Scenedata
{
	mat4 projMat;
	mat4 viewMat;
	float totalTime;
	float deltaTime;
} SceneD;

uniform mat4 modelMat;

out VertexOut
{
   vec4 WorldPos;
   vec3 Normal;
   mat3 TBN;
   vec2 TexCoords;
} OUT;

void main()
{
	OUT.Normal = vNormal;

	OUT.WorldPos = modelMat * vec4(vPos, 1);

	OUT.TexCoords = vTexCoord;

	vec3 T = normalize(vec3(modelMat * vec4(vTangent, 0)));
	// Normals buggy otherwise
    vec3 N = normalize(vec3(modelMat * vec4(vNormal, 0)));
	vec3 B = normalize(vec3(modelMat * vec4(vBiTangent, 0)));
	OUT.TBN = mat3(T, B, N);

	gl_Position = SceneD.projMat * SceneD.viewMat * OUT.WorldPos;
	
}