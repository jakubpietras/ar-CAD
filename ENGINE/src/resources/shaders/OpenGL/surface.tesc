#version 450 core

uniform uint u_SamplesU, u_SamplesV;
uniform bool u_SwitchCoords;

layout (vertices = 16) out;

in vec3 PosControl[];
flat in uint IDControl[];

// data which goes out tessellation control and into evaluation
out vec3 PosEval[];
patch out uint IDEval;


void main()
{
	PosEval[gl_InvocationID] = PosControl[gl_InvocationID];
	if (gl_InvocationID == 0)
		IDEval = IDControl[gl_InvocationID];

	if (gl_InvocationID == 0) 
    {
        if (!u_SwitchCoords) 
        {
            gl_TessLevelOuter[0] = u_SamplesU + 1;
            gl_TessLevelOuter[1] = u_SamplesV + 1;
        } 
        else 
        {
            gl_TessLevelOuter[0] = u_SamplesV + 1;
            gl_TessLevelOuter[1] = u_SamplesU + 1;    
        }
    }
}
