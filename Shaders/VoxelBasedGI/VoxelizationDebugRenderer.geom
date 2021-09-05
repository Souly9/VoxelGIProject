#version 430 core
#extension GL_ARB_shading_language_include : require

layout(points)in;
layout(triangle_strip, max_vertices=24)out;

#include "/ConeTracingGlobals.glsl"

layout(std140,binding=10)uniform
Scenedata
{
    mat4 projMat;
    mat4 viewMat;
    float totalTime;
    float deltaTime;
}SceneD;

in VertexOut
{
    vec4 worldPos;
    vec3 Color;
    uint empty;
}IN[];

out GSOut{
    vec3 Color;
}OUT;

void main()
{
    float size= VoxelUniforms.VoxelSize;
    OUT.Color=IN[0].Color;
	// Procedurally generate vertices for a cube
	if(IN[0].empty > 0)
	{
        vec3 worldPos=gl_in[0].gl_Position.xyz;
        
		vec4 pos = vec4(worldPos + vec3(-0.5, -0.5, -0.5)* size , 1);
		gl_Position=SceneD.projMat*SceneD.viewMat * pos;
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 1, 0)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(1, 0, 0)* size), 1);
		EmitVertex();	
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(1, 1, 0)* size), 1);
		EmitVertex();
		EndPrimitive();

		pos = vec4(worldPos + vec3(0.5, -0.5, -0.5)* size, 1);		
		gl_Position=SceneD.projMat*SceneD.viewMat*(pos);	
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 1, 0)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 0, 1)* size), 1);
		EmitVertex();	
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 1, 1)* size), 1);
		EmitVertex();
		EndPrimitive();

		pos = vec4(worldPos + vec3(0.5, -0.5, 0.5)* size, 1);	
		gl_Position=SceneD.projMat*SceneD.viewMat*(pos);		
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 1, 0)* size), 1);
		EmitVertex();											
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(-1, 0, 0)* size), 1);
		EmitVertex();											
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(-1, 1, 0)* size), 1);
		EmitVertex();
		EndPrimitive();

		pos = vec4(worldPos + vec3(-0.5, -0.5, 0.5)* size, 1);	
		gl_Position=SceneD.projMat*SceneD.viewMat*(pos);	
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 1, 0)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 0, -1)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 1, -1)* size), 1);
		EmitVertex();
		EndPrimitive();

		pos = vec4(worldPos + vec3(-0.5, -0.5, 0.5)* size, 1);	
		gl_Position=SceneD.projMat*SceneD.viewMat*(pos);		
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(1, 0, 0)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0 ,0, -1)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(1, 0, -1)* size), 1);
		EmitVertex();
		EndPrimitive();

		pos = vec4(worldPos + vec3(-0.5, 0.5, -0.5)* size, 1);
		gl_Position=SceneD.projMat*SceneD.viewMat*(pos);	
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(0, 0, 1)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(1, 0, 0)* size), 1);
		EmitVertex();
		gl_Position=SceneD.projMat*SceneD.viewMat*vec4((pos.xyz + vec3(1, 0, 1)* size), 1);
		EmitVertex();
    
    EndPrimitive();
	}
}