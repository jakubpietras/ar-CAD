#version 450 core

in vec2 UVFrag;

out vec4 FragColor;  

uniform vec3 u_Color;
uniform sampler2D u_TrimTex;
uniform bool u_RenderSideA; // A - filled, B - unfilled
uniform bool u_ShouldTrim;

const vec3 FilledColor = vec3(1.0, 0.0, 0.0);

void main()
{
	if (u_ShouldTrim)
	{
		vec3 trimColor = texture(u_TrimTex, UVFrag).rgb;

		bool isFilled = distance(trimColor, FilledColor) < 0.01;
		if (u_RenderSideA && !isFilled) discard;
		if (!u_RenderSideA &&  isFilled) discard;
	}

	FragColor = vec4(u_Color, 1.0);
}