#version 450 core

layout (location = 0) in vec3 a_Position;  
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_VP;

out vec3 WorldPosFrag;
out vec3 NormFrag;

uniform sampler2D u_Heightmap;

void main()
{
	NormFrag = a_Normal;
	WorldPosFrag = a_Position + NormFrag * texture(u_Heightmap, a_TexCoord).r;
	gl_Position = u_VP * vec4(WorldPosFrag, 1.0);
}