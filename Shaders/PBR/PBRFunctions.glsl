const float PI = 3.14159265359;
// Hardcoded radius for pointlights for simplification
const float PointLightRadius = 3.0;

// Epic's attentuation function from UE4
float compAttentuation(vec3 worldPos, vec3 lightPos)
{
	float dist=length(lightPos-worldPos);

	float num = clamp(1.0 - pow(pow((dist / PointLightRadius), 4.0), 2.0), 0.0, 1.0);
	return num / ((dist * dist) + 1.0);
}

/*
* Implementation of Epic's UE4 BRDF
*/

vec3 fresnelSchlick(float cosTheta, vec3 materialReflection)
{
	return materialReflection + (1.0 - materialReflection) * pow(2.0, -5.55473 * cosTheta-6.98316) * cosTheta;
}

float DistributionGGX(vec3 normal, vec3 halfway, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, halfway),0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
	float NdotV = max(dot(normal, viewDir), 0.0);
	float NdotL = max(dot(normal, lightDir), 0.0);

	float r = roughness + 1.0;
	float k = (r * r) / 8.0; 

	float schlickV = GeometrySchlickGGX(NdotV, k);
	float schlickL = GeometrySchlickGGX(NdotL, k);
	return (schlickV * schlickL);

}

vec3 computePointLight(vec3 worldPos, vec3 lightPos, vec3 viewDir, vec3 normal, 
	vec3 halfwayDir, vec3 lightColor, vec3 diffuse, float roughness)
{
	vec3 wi = normalize(lightPos - worldPos);
	float cosTheta = max(dot(normal, wi), 0.0);
	float attentuation = compAttentuation(worldPos, lightPos);

	vec3 radiance = (lightColor * attentuation);
	// Hardcoded for everything for now
	vec3 materialReflection = vec3(0.04);
	
	vec3 Fresnel = vec3(0);

	// Clamp dot product
	Fresnel = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), materialReflection);

	float NDF = DistributionGGX(normal, halfwayDir, roughness);
	float G = GeometrySmith(normal, viewDir, wi, roughness);

	vec3 numerator = NDF * G * Fresnel;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, wi), 0.0);
	vec3 specular = vec3(0);
	// floored to delta to prevent artifacts
	specular = numerator / max(denominator, 0.000001);

	// No metallic support
	vec3 kD = vec3(1.0) - Fresnel;

	float NdotL = max(dot(normal, wi), 0.0);
	return (kD * (diffuse / PI) + specular) * radiance * NdotL;
}

vec3 computeAmbient(vec3 color)
{
	return Lights.ambientStrength * color;
}

// Implemented with https://www.shadertoy.com/view/lslGzl
vec3 ModifiedReinhardTMO(vec3 color)
{
	float exposure = ConeTraceData.lightingSettings.x;
	color *= exposure;
	float white = 2.4;
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1.0 + luma / (white*white)) / (1.0 + luma);
	color *= toneMappedLuma / luma;
	return color;
}

vec3 AcesTMO(vec3 color)
{
	float exposure = ConeTraceData.lightingSettings.x;
	color *= exposure;
	float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((color*(a*color+b))/(color*(c*color+d)+e), 0.0, 1.0);
}

// Implemented based off the presentation and https://www.shadertoy.com/view/lslGzl
vec3 Uncharted2TMO(vec3 color)
{
    float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = ConeTraceData.lightingSettings.x;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	return color;
}