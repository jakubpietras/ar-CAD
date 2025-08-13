#version 450 core
layout(location = 0) out uint outID; 
  
flat in uint id;

void main()
{
	outID = id;
}