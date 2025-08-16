#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in uint a_ID;

out vec3 PosControl;
flat out uint IDControl;

void main()
{
	// no transformations applied on the CONTROL POINTS
	PosControl = a_Position;
	IDControl = a_ID;
	gl_Position = vec4(a_Position, 1.0);
}