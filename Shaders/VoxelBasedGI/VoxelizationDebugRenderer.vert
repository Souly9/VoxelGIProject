#version 440 core
#extension GL_ARB_shading_language_include : require

uniform sampler3D colorTex3DFirstBounce;

#include "/ConeTracingGlobals.glsl"

out VertexOut
{
    vec4 worldPos;
    vec3 Color;
    uint empty;
}OUT;

ivec3 unflattenPosition(uint arrayPos)
{
    return ivec3(
        arrayPos%VoxelUniforms.VoxelDimensions,
        (arrayPos/VoxelUniforms.VoxelDimensions)%VoxelUniforms.VoxelDimensions,
        arrayPos/(VoxelUniforms.VoxelDimensions*VoxelUniforms.VoxelDimensions)
    );
}

void main()
{
    ivec3 intPos=unflattenPosition(gl_VertexID);
    vec3 worldSpacePos = VoxelToWorld(intPos);
    ivec3 readPos = ivec3(intPos);
    vec4 c =texture(colorTex3DFirstBounce, worldSpacePos.xyz / (VoxelUniforms.VoxelDimensions / VoxelUniforms.VoxelsPerUnit));
    OUT.Color = c.rgb;
    OUT.worldPos = vec4(worldSpacePos, 0);
    OUT.empty = uint(c.a);
    gl_Position=vec4(worldSpacePos, 0);
    
}