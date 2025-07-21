#version 450 core

layout (location = 0) in vec3 a_Position;   

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_InvView;
uniform mat4 u_InvProjection;

vec3 unprojectPoint(float x, float y, float z, mat4 invView, mat4 invProjection) 
{
    vec4 unprojectedPoint =  invView * invProjection * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

out vec3 nearPoint;
out vec3 farPoint;
out mat4 fragView;
out mat4 fragProjection;

void main()
{
    vec3 p = a_Position;
    mat4 invView = inverse(u_View);
    mat4 invProj = inverse(u_Projection);

    fragView = u_View;
    fragProjection = u_Projection;
    nearPoint = unprojectPoint(p.x, p.y, 0.0f, invView, invProj);
    farPoint = unprojectPoint(p.x, p.y, 1.0f, invView, invProj);
    gl_Position = vec4(p, 1.0f);
}