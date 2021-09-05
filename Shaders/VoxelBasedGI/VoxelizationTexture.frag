#version 440 core
#extension GL_ARB_bindless_texture:enable
#extension GL_ARB_shading_language_include : require

#include "/Globals.glsl"
#include "/ConeTracingGlobals.glsl"
#include "/PBRFunctions.glsl"

layout(std140,binding=10)uniform
Scenedata
{
	mat4 projMat;
	mat4 viewMat;
	float totalTime;
	float deltaTime;
}SceneD;

in GeomOut
{
	vec3 WorldPos;
	vec3 Color;
	vec3 Normal;
	mat3 TBN;
	vec2 TexCoords;
}IN;

uniform Material material;
uniform sampler2D opaque;
uniform sampler2D normalMap;

layout(rgba16f, binding = 0) uniform image3D color3DTex;
layout(rgba16f, binding = 1) uniform image3D normal3DTex;

void main()
{
	// Take albedo and normal from textures in this case, rest remains identical to Voxelization.frag
	vec3 albedo=texture(opaque, IN.TexCoords).xyz;
	vec4 diffuseLighting = vec4(0);
	diffuseLighting.xyz += computeAmbient(albedo);

	vec3 viewDir=normalize(Lights.cameraPos-IN.WorldPos.xyz);

	vec3 normal = texture(normalMap, IN.TexCoords).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(IN.TBN * normal);
	normal = normalize(IN.Normal);

	for(int i=0;i<MAX_LIGHTS;++i)
	{
		PointLight light=Lights.lights[i];
		vec3 lightPos=light.pos.xyz;
		vec3 lightDir = normalize(lightPos - IN.WorldPos.xyz);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		float attenuation=compAttentuation(IN.WorldPos.xyz, lightPos);
		
		vec3 reflectDir=reflect(-lightDir,normal);
			
		float diff=max(dot(normal,lightDir),0);
		vec3 diffuse=diff*albedo;
			
		diffuse*=attenuation;

		diffuseLighting.xyz += diffuse;
	}
	ivec3 worldPos=WorldToVoxel(IN.WorldPos.xyz);
	diffuseLighting.a = 1;

	vec4 oldVal = imageLoad(color3DTex, worldPos);
	diffuseLighting = max(oldVal, diffuseLighting);

	imageStore(color3DTex, worldPos, diffuseLighting);
	imageStore(normal3DTex, worldPos, vec4(normal, 1.0 - material.specular.x));
}