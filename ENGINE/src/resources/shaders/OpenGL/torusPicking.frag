#version 450 core
layout(location = 0) out uint outID; 
  
flat in uint id;
in vec2 UVFrag;

void main()
{
	outID = id;
}