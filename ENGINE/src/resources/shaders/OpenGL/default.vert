#version 450 core
layout (location = 0) in vec3 a_Position;   
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec3 a_Offset;
layout (location = 3) in mat4 a_Model;

uniform mat4 u_VP;
  
out vec3 color;

void main()
{
	gl_Position = u_VP * a_Model * vec4(a_Position + a_Offset, 1.0);
	color = a_Color; 
}     