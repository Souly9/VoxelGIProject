#version 440 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_bindless_texture : enable
#extension GL_NV_gpu_shader5 : enable

#include "/Globals.glsl"

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec3 gNormal;
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
uniform sampler2D opaque;
uniform sampler2D normalMap;
uniform sampler2D displacement;
uniform sampler2D specularMap; 

void main()
{
    gPosition = vec4(IN.WorldPos.xyz, 0);

    vec3 normal = texture(normalMap, IN.TexCoords).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(IN.TBN * normal);
	normal = normalize(IN.Normal);

	gNormal = normal;
    gDiffuseRough.xyz = texture(opaque, IN.TexCoords).xyz;
    gDiffuseRough.a = sqrt(4 / (material.shininess + 2));
}