#version 450 core  
layout (location = 0) in mat4 a_Model;

uniform mat4 u_VP;
  
void main()
{
	gl_Position = u_VP * a_Model * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}     