#version 450 core
layout(location = 0) out uint outID; 

uniform sampler2D u_TrimTex;
uniform bool u_RenderSideA; // A - filled, B - unfilled
uniform bool u_ShouldTrim;

flat in uint id;
in vec2 UVFrag;

const vec3 FilledColor = vec3(1.0, 0.0, 0.0);

void main()
{
	if (u_ShouldTrim)
	{
		vec3 trimColor = texture(u_TrimTex, UVFrag).rgb;

		if (u_RenderSideA && (trimColor != FilledColor))
				discard;
		if (!u_RenderSideA && (trimColor == FilledColor))
				discard;
	}

	outID = id;
}