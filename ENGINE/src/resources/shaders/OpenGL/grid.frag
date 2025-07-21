#version 410 core

in vec3 nearPoint;
in vec3 farPoint;
in mat4 fragView;
in mat4 fragProjection;

out vec4 FragColor;

vec4 grid(vec3 fragPos3D, float scale) 
{
    vec2 coord = fragPos3D.xz * scale; // scale - distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.2 * minimumx && fragPos3D.x < 0.2 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.2 * minimumz && fragPos3D.z < 0.2 * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) 
{
    vec4 clip_space_pos = fragProjection * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) 
{
    vec4 clip_space_pos = fragProjection * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * 100 * 0.1) / (100 + 0.1 - clip_space_depth * (100 - 0.1)); // get linear value between 0.01 and 100
    return linearDepth / 100; // normalize
}

void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    float opacity = t > 0 ? 1.0f : 0.0f;
    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    vec4 color = (grid(fragPos3D, 5) + grid(fragPos3D, 1)) * opacity;

    FragColor = color * fading;
}