#version 420 core
layout (location = 0) out vec4 gWorldPos;
layout(location = 3) out vec4 gDiffuseSpec;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    gDiffuseSpec.xyz = texture(skybox, TexCoords).rgb;
    // w component used in deferred shader to disable shading
    gWorldPos = vec4(1);
}