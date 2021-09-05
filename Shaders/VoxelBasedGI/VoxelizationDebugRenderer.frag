#version 430 core
#extension GL_ARB_shading_language_include : require

#include "/Globals.glsl"
#include "/ConeTracingGlobals.glsl"
#include "/PBRFunctions.glsl"

in GSOut{
    vec3 Color;
}IN;

layout (location = 0) out vec4 gWorldPos;
layout(location = 3) out vec4 gDiffuseSpec;

out vec4 FragColor;

void main()
{
	// Choose tonemapping operator
	vec3 rslt = vec3(0);
	if(ConeTraceData.lightingSettings.z == 0)
		rslt = ModifiedReinhardTMO(IN.Color);
	else if(ConeTraceData.lightingSettings.z == 1)
		rslt = AcesTMO(IN.Color);
	else if(ConeTraceData.lightingSettings.z == 2)
		rslt = Uncharted2TMO(IN.Color);
    gDiffuseSpec.xyz = rslt;
    gWorldPos = vec4(1);
}