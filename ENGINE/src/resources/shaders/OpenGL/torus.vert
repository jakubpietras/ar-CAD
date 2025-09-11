#version 450 core
layout (location = 0) in vec3 a_Position;  
layout (location = 1) in vec2 a_UV;
layout (location = 2) in uint a_ID;

flat out uint id;
out vec2 UVFrag;

uniform mat4 u_Model;
uniform mat4 u_VP;
  
void main()
{
	gl_Position = u_VP * u_Model * vec4(a_Position, 1.0);
	id = a_ID;
	UVFrag = a_UV;
}     