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

layout(rgba16f, binding = 0) uniform image3D color3DTex;
layout(rgba16f, binding = 1) uniform image3D normal3DTex;

void main()
{
	// Color comes from material and normal from geometry
	vec3 albedo=material.diffuse;
	vec4 diffuseLighting = vec4(0);
	diffuseLighting.xyz += computeAmbient(albedo);

	vec3 viewDir=normalize(Lights.cameraPos-IN.WorldPos.xyz);
	vec3 normal = IN.Normal;
	
	// Simple Blinn-Phong without specular for direct light
	for(int i=0;i<MAX_LIGHTS;++i)
	{
		PointLight light=Lights.lights[i];
		vec3 lightPos=light.pos.xyz;
		vec3 lightDir = normalize(lightPos - IN.WorldPos.xyz);

		float attenuation=compAttentuation(IN.WorldPos.xyz, lightPos);
		
		float diff=max(dot(normal,lightDir),0);
		vec3 diffuse=diff*albedo*light.color.xyz;
			
		diffuse*=attenuation;

		diffuseLighting.xyz += diffuse;
	}

	ivec3 worldPos=WorldToVoxel(IN.WorldPos.xyz);
	// Add emissive to color
	diffuseLighting.xyz += material.emissive;

	diffuseLighting.a = 1;
	// Choose tonemapping operator
	vec4 oldVal = imageLoad(color3DTex, worldPos);
	// Prevents extreme flickering of values with rapid re-voxelization
	diffuseLighting = max(oldVal, diffuseLighting);

	imageStore(color3DTex, worldPos, diffuseLighting);
	imageStore(normal3DTex, worldPos, vec4(normal, 0));
}