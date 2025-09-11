#version 450 core

layout (isolines, equal_spacing) in;

in vec3 PosEval[];
patch in uint IDEval;

flat out uint IDFrag;
out vec2 UVFrag;

uniform mat4 u_Model, u_VP;
uniform uint u_SamplesU, u_SamplesV;
uniform bool u_SwitchCoords;

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
	float u = u_SwitchCoords ? gl_TessCoord.x : gl_TessCoord.y;
	float v = u_SwitchCoords ? gl_TessCoord.y : gl_TessCoord.x;

	u = clamp(u * u_SamplesU / (u_SamplesU - 1), 0.0, 1.0);
	v = clamp(v * u_SamplesV / (u_SamplesV - 1), 0.0, 1.0);

	vec3 NewPos[16];
	for (int i = 0; i < 5; i++)
		NewPos[i] = PosEval[i];
	NewPos[5] = (u != 0 && v != 0) ? (u * PosEval[6] + v * PosEval[5]) / (u + v) : (PosEval[6] + PosEval[5]) / 2;
	NewPos[6] = (u != 1 && v != 0) ? ((1 - u) * PosEval[7] + v * PosEval[8]) / (1 - u + v) : (PosEval[8] + PosEval[7]) / 2;
	NewPos[7] = PosEval[9];
	NewPos[8] = PosEval[10];
	NewPos[9] = (u != 0 && v != 1) ? (u * PosEval[12] + (1 - v) * PosEval[11]) / (1 + u - v) : (PosEval[11] + PosEval[12]) / 2;
	NewPos[10] = (u != 1 && v != 1) ? ((1 - u) * PosEval[13] + (1 - v) * PosEval[14]) / (2 - u - v) : (PosEval[14] + PosEval[13]) / 2;
	NewPos[11] = PosEval[15];
	NewPos[12] = PosEval[16];
	NewPos[13] = PosEval[17];
	NewPos[14] = PosEval[18];
	NewPos[15] = PosEval[19];

	// --------------
	vec3 point = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int pointIdx = i * 4 + j;
			point += NewPos[pointIdx] * CubicBernstein(i, u) * CubicBernstein(j, v);
		}
	}
	vec4 pointPos = u_VP * u_Model * vec4(point, 1.0f);
	gl_Position = pointPos;
	IDFrag = IDEval;
	UVFrag = vec2(u, v);
}