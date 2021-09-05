#pragma once
#define GLFW_INCLUDE_NONE

/********** Constants **********/
#define COMBUSTION_GL_VERSION_MAJOR 4
#define COMBUSTION_GL_VERSION_MINOR 4

#define MAYBE_UNUSED(e) static_cast<void>(e)

// Uniform buffer index defines
#define COMBUSTION_SCENE_BUFFER_INDEX 10
#define COMBUSTION_MODEL_BUFFER_INDEX 11
#define COMBUSTION_VOXEL_UNIFORM_BUFFER 12
#define COMBUSTION_GENERIC_LIGHT_BUFFER_INDEX 30

#define COMBUSTION_AMBIENT_STRENGTH 0.1f
#define COMBUSTION_MAX_LIGHTS 10
#define COMBUSTION_TEXTURE_LIMIT 30
#define COMBUSTION_FLOAT_TOLERANCE 1e-6f

// Voxel GI variable
#define VOXEL_INITIAL_DIMENSIONS 128
#define COMBUSTION_CONE_TRACING_BUFFER_INDEX 50

// Shader Include Globals
namespace VoxelIncludeConstants
{
	const int coneTracingLengths[] = {27, 25, 14, 19};
	const char* const coneTracingIncludes[] = {
		"/ConeTracingFunctions.glsl", "/ConeTracingGlobals.glsl", "/Globals.glsl", "/PBRFunctions.glsl"
	};
	const int coneTracingNumOfIncludes = 4;
}
