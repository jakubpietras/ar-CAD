#version 410 core

uniform uint u_SamplesU, u_SamplesV;
uniform bool u_SwitchCoords;

layout (vertices = 16) out;

in vec3 PosControl[];    // de Boor points
out vec3 PosEval[];

void main()
{
	if (!u_SwitchCoords)
	{
		gl_TessLevelOuter[0] = u_SamplesU + 1; // how many isolines
		gl_TessLevelOuter[1] = u_SamplesV + 1; // how many times split each line
	}
	else
	{
		gl_TessLevelOuter[0] = u_SamplesV + 1; // how many isolines
		gl_TessLevelOuter[1] = u_SamplesU + 1; // how many times split each line
	}
    
    vec3 p[16], // intermediate Bezier points
        B[16];  // Bezier points

    for (int y = 0; y < 4; y++)
    {
        p[y * 4 + 0] = (PosControl[y * 4 + 0] + 4.0 * PosControl[y * 4 + 1] + PosControl[y * 4 + 2]) / 6.0;
        p[y * 4 + 1] = (4.0 * PosControl[y * 4 + 1] + 2.0 * PosControl[y * 4 + 2]) / 6.0;
        p[y * 4 + 2] = (2.0 * PosControl[y * 4 + 1] + 4.0 * PosControl[y * 4 + 2]) / 6.0;
        p[y * 4 + 3] = (PosControl[y * 4 + 1] + 4.0 * PosControl[y * 4 + 2] + PosControl[y * 4 + 3]) / 6.0;
    }
    for (int x = 0; x < 4; x++)
    {
        B[0 * 4 + x] = (p[0 * 4 + x] + 4.0 * p[1 * 4 + x] + p[2 * 4 + x]) / 6.0;
        B[1 * 4 + x] = (4.0 * p[1 * 4 + x] + 2.0 * p[2 * 4 + x]) / 6.0;
        B[2 * 4 + x] = (2.0 * p[1 * 4 + x] + 4.0 * p[2 * 4 + x]) / 6.0;
        B[3 * 4 + x] = (p[1 * 4 + x] + 4.0 * p[2 * 4 + x] + p[3 * 4 + x]) / 6.0;
    }
    PosEval[gl_InvocationID] = B[gl_InvocationID];
}