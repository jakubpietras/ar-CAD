#version 450 core
layout (location = 0) in vec3 a_Position;  
layout (location = 1) in uint a_ID;

flat out uint id;

uniform mat4 u_Model;
uniform mat4 u_VP;
  
void main()
{
	gl_Position = u_VP * u_Model * vec4(a_Position, 1.0);
	id = a_ID;
}     