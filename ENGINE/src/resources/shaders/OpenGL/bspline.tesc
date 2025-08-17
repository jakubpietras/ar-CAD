#version 450 core

uniform vec2 u_Viewport;

// specifies how many vertices there are per patch (set on the CPU)
layout (vertices = 4) out;

// data which goes into tessellation control
in vec3 PosControl[];
flat in uint IDControl[];

// data which goes out tessellation control and into evaluation
out vec3 PosEval[];
flat out uint IDEval[];

void main()
{
	// de Boor -> Bezier conversion
	vec4 P0 = gl_in[0].gl_Position,		// P[i - 2]
	P1 = gl_in[1].gl_Position,			// P[i - 1]
	P2 = gl_in[2].gl_Position,			// P[i]
	P3 = gl_in[3].gl_Position;			// P[i + 1]

	// gl_InvocationID tracks which vertex of the patch we are currently processing
	if (gl_InvocationID == 0) 
        PosEval[gl_InvocationID] = ((1.0/6.0) * (P0 + 4.0 * P1 + P2)).xyz;
    else if (gl_InvocationID == 1) 
        PosEval[gl_InvocationID] = ((1.0/6.0) * (4.0 * P1 + 2.0 * P2)).xyz;
    else if (gl_InvocationID == 2) 
        PosEval[gl_InvocationID] = ((1.0/6.0) * (2.0 * P1 + 4.0 * P2)).xyz;
    else if (gl_InvocationID == 3) 
        PosEval[gl_InvocationID] = ((1.0/6.0) * (P1 + 4.0 * P2 + P3)).xyz;
    
	IDEval[gl_InvocationID] = IDControl[gl_InvocationID];

	// adaptive drawing
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

	int Segments = int(length(MaxPoint - MinPoint));

	// tessellation levels
	gl_TessLevelOuter[0] = 1;				// how many lines
	gl_TessLevelOuter[1] = Segments;		// how many segments for each line
}