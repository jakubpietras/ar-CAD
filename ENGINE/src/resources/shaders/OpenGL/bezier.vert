#version 450 core

layout (location = 0) in vec3 a_Pos;

out vec3 PosControl;

void main()
{
	// no transformations applied on the CONTROL POINTS
	PosControl = a_Pos;
}