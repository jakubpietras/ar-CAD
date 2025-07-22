#version 410 core

uniform mat4 u_VP;

const vec3 verts[6] = vec3[](
    vec3(1.0, 0.0, 1.0),
    vec3(-1.0, 0.0, -1.0),
    vec3(-1.0, 0.0, 1.0),
    vec3(-1.0, 0.0, -1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 0.0, -1.0)
);

out vec2 v_Coords;

void main()
{
    vec3 p = verts[gl_VertexID] * 1000;
    v_Coords = p.xz;
    gl_Position = u_VP * vec4(p, 1.0f);
}
