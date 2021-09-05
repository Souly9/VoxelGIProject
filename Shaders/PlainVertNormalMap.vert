#version 420 
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiTangent;

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
   vec4 vertPos;
   vec4 WorldPos;
   vec4 ScreenPos;
   mat4 invNormal;
   float delta;
   vec3 Normal;
   mat3 TBN;
   vec2 TexCoords;
   } OUT;

void main()
{
	OUT.Normal = vNormal;
	OUT.delta = SceneD.deltaTime;

	OUT.WorldPos = modelMat * vec4(vPos, 1);
	OUT.ScreenPos =  SceneD.viewMat * OUT.WorldPos;
	OUT.vertPos = SceneD.projMat * OUT.ScreenPos;

	OUT.TexCoords = vTexCoord;

	vec3 T = normalize(vec3(modelMat * vec4(vTangent,   1.0)));
	// Normals buggy otherwise
	vec3 N = normalize(vec3(modelMat * vec4(vNormal, 1.0)));
	vec3 B = normalize(vec3(modelMat * vec4(vBiTangent, 1.0)));
	
	OUT.TBN = mat3(T, B, N);
	OUT.invNormal = inverse(SceneD.projMat);
	gl_Position = OUT.vertPos;
	
}