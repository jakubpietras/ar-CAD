#version 450 core
layout (location = 0) in vec3 a_Position;   
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec3 a_Offset;

uniform mat4 u_VP;
  
out vec3 color;

void main()
{
	gl_Position = u_VP * vec4(a_Position + a_Offset, 1.0);
	color = a_Color; 
}     