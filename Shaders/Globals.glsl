#define MAX_LIGHTS 2

struct PointLight
{
	vec4 pos;
	vec4 color;
};

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 emissive;
	float shininess;
};

layout(std140,binding=30)uniform
LightData
{
	float ambientStrength;
	vec3 cameraPos;
	PointLight lights[MAX_LIGHTS];
}Lights;
