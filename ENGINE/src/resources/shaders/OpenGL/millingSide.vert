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
	if (a_TexCoord.x < -0.5 || a_TexCoord.y < -0.5)
		WorldPosFrag = a_Position;
	else
		WorldPosFrag = a_Position + vec3(0.0f, 1.0f, 0.0f) * texture(u_Heightmap, a_TexCoord).r;
	gl_Position = u_VP * vec4(WorldPosFrag, 1.0);
}