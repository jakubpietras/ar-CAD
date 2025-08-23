#version 450 core

uniform mat4 u_VP;
uniform mat4 u_Model;

// abstract patch type + spacing used
layout (isolines, equal_spacing) in;

// control points passed from tessellation control
in vec3 PosEval[];
patch in uint IDEval;

flat out uint IDFrag;

// nothing to output to fragment shader

vec3 ComputeBernstein(vec3 b0, vec3 b1, vec3 b2, vec3 b3, float t)
{
	// de Casteljau
    vec3 coeff[] = { b0, b1, b2, b3 };
	for (int j = 3; j > 0; j--)
        for (int i = 0; i < j; i++)
            coeff[i] = (1 - t) * coeff[i] + t * coeff[i + 1];
    return coeff[0];
}

vec3 ComputeBernstein(vec3 b0, vec3 b1, vec3 b2, float t)
{
	// de Casteljau
    vec3 coeff[] = { b0, b1, b2 };
	for (int j = 2; j > 0; j--)
        for (int i = 0; i < j; i++)
            coeff[i] = (1 - t) * coeff[i] + t * coeff[i + 1];
    return coeff[0];
}

bool PointsEqual(vec3 p1, vec3 p2, float epsilon) 
{
    return distance(p1, p2) < epsilon;
}
const float Epsilon = 0.0001f;

void main()
{
	// parameter obtained from the tessellator
	float t = gl_TessCoord.x;
    
    // CONVENTION USED: 
    // an input patch will always have 4 points:
    // - if patch has points: [A, A, B, B], then the curve becomes a line
    // - if patch has points: [A, B, C, C], then the curve is quadratic
    // - if patch has points: [A, B, C, D], then the curve is cubic

    vec3 CurvePoint;

    if (PointsEqual(PosEval[0], PosEval[1], Epsilon) && PointsEqual(PosEval[2], PosEval[3], Epsilon))
    {
        // Line
        CurvePoint = PosEval[0] + (PosEval[2] - PosEval[0]) * t;
    }
    else if (PointsEqual(PosEval[2], PosEval[3], Epsilon))
    {
        // Quadratic
        CurvePoint = ComputeBernstein(PosEval[0], PosEval[1], PosEval[2], t);
    }
    else
    {
        // Cubic
        CurvePoint = ComputeBernstein(PosEval[0], PosEval[1], PosEval[2], PosEval[3], t);
    }
    gl_Position = u_VP * u_Model * vec4(CurvePoint, 1.0f);
    IDFrag = IDEval;
}