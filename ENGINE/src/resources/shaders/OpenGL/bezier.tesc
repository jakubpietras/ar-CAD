#version 450 core

uniform vec2 u_Viewport;

// specifies how many vertices there are per patch (set on the CPU)
layout (vertices = 4) out;

// data which goes into tessellation control
in vec3 PosControl[];

// data which goes out tessellation control and into evaluation
out vec3 PosEval[];

void main()
{
	// gl_InvocationID tracks which vertex of the patch we are currently processing
	PosEval[gl_InvocationID] = PosControl[gl_InvocationID];

	vec4 P0 = gl_in[0].gl_Position, 
	P1 = gl_in[1].gl_Position,
	P2 = gl_in[2].gl_Position, 
	P3 = gl_in[3].gl_Position;

	vec3 P0_NDC = P0.xyz / P0.w, 
	P1_NDC = P1.xyz / P1.w,
	P2_NDC = P2.xyz / P2.w, 
	P3_NDC = P3.xyz / P3.w;

	vec2 ScreenP0 = (P0_NDC.xy * 0.5 + 0.5) * u_Viewport,
	ScreenP1 = (P1_NDC.xy * 0.5 + 0.5) * u_Viewport,
	ScreenP2 = (P2_NDC.xy * 0.5 + 0.5) * u_Viewport,
	ScreenP3 = (P3_NDC.xy * 0.5 + 0.5) * u_Viewport;

	vec2 MinPoint = min(min(ScreenP0, ScreenP1), min(ScreenP2, ScreenP3));
	vec2 MaxPoint = max(max(ScreenP0, ScreenP1), max(ScreenP2, ScreenP3));
	vec2 Diagonal = MaxPoint - MinPoint;

	int Segments = max(int(Diagonal.x * Diagonal.y), 4);

	// tessellation levels
	gl_TessLevelOuter[0] = 1;				// how many lines
	gl_TessLevelOuter[1] = Segments;		// how many segments for each line
}