#version 450 core

layout (isolines, equal_spacing) in;

in vec3 PosEval[];
patch in uint IDEval;

flat out uint IDFrag;
out vec2 UVFrag;

uniform mat4 u_Model, u_VP;
uniform uint u_SamplesU, u_SamplesV;
uniform bool u_SwitchCoords;
uniform uint u_TotalSegmentsU, u_TotalSegmentsV; // Total segments in the surface
uniform uint u_CurrentSegmentU, u_CurrentSegmentV; // Current patch position

float CubicBernstein(int i, float t)
{
	if (i == 0)
		return 1.0f - 3 * t + 3 * t * t - t * t * t;
	if (i == 1)
		return 3 * t - 6 * t * t + 3 * t * t * t;
	if (i == 2)
	    return 3 * t * t - 3 * t * t * t;
	if (i == 3)
		return t * t * t;
	return 0;
}

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	if (u_SwitchCoords) {
		float temp = u;
		u = v;
		v = temp;
	}

	float segmentWidth = 1.0f / float(u_TotalSegmentsU);
    float segmentHeight = 1.0f / float(u_TotalSegmentsV);
    
    float globalU = (float(u_CurrentSegmentU) + u) * segmentWidth;
    float globalV = (float(u_CurrentSegmentV) + v) * segmentHeight;
    
    UVFrag = vec2(globalU, globalV);

	float bezier_u = clamp(u * u_SamplesU / (u_SamplesU - 1), 0.0, 1.0);
	float bezier_v = clamp(v * u_SamplesV / (u_SamplesV - 1), 0.0, 1.0);

	vec3 point = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int pointIdx = i * 4 + j;
			point += PosEval[pointIdx] * CubicBernstein(i, bezier_u) * CubicBernstein(j, bezier_v);
		}
	}
	vec4 pointPos = u_VP * u_Model * vec4(point, 1.0f);
	gl_Position = pointPos;
	IDFrag = IDEval;
	UVFrag = vec2(u, v);
}