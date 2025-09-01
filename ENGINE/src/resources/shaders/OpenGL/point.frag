#version 450 core
out vec4 FragColor;  
in vec3 ColorFrag;
  
void main()
{
	FragColor = vec4(ColorFrag, 1.0f);
}