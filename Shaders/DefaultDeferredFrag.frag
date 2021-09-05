#version 440 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_bindless_texture : enable
#extension GL_NV_gpu_shader5 : enable

#include "/Globals.glsl"

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gEmissive;
layout(location = 3) out vec4 gDiffuseRough;

out vec4 FragColor;

in VertexOut
{
   vec4 WorldPos;
   vec3 Normal;
   mat3 TBN;
   vec2 TexCoords;
} IN;

uniform Material material;

void main()
{
    gPosition = vec4(IN.WorldPos.xyz, 0);
	gNormal = IN.Normal;
    gEmissive = material.emissive;
    gDiffuseRough.xyz = material.diffuse;
    // Map shininess to roughness
    gDiffuseRough.a = sqrt(4 / (material.shininess + 2));
}