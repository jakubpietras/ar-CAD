#version 410 core

const vec2 width = vec2(0.03f, 0.03f);
const float majorAxisThickness = 0.02f;

out vec4 FragColor;
in vec2 v_Coords;

// https://www.shadertoy.com/view/mdVfWw?source=post_page-----727f9278b9d8
// Author: Ben Golus

float pristineGrid(vec2 uv, vec2 lineWidth)
{
    vec2 ddx = dFdx(uv);
    vec2 ddy = dFdy(uv);
    vec2 uvDeriv = vec2(length(vec2(ddx.x, ddy.x)), length(vec2(ddx.y, ddy.y)));
    bvec2 invertLine = bvec2(lineWidth.x > 0.5, lineWidth.y > 0.5);
    vec2 targetWidth = vec2(
      invertLine.x ? 1.0 - lineWidth.x : lineWidth.x,
      invertLine.y ? 1.0 - lineWidth.y : lineWidth.y
      );
    vec2 drawWidth = clamp(targetWidth, uvDeriv, vec2(0.5));
    vec2 lineAA = uvDeriv * 1.5;
    vec2 gridUV = abs(fract(uv) * 2.0 - 1.0);
    gridUV.x = invertLine.x ? gridUV.x : 1.0 - gridUV.x;
    gridUV.y = invertLine.y ? gridUV.y : 1.0 - gridUV.y;
    vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);

    grid2 *= clamp(targetWidth / drawWidth, 0.0, 1.0);
    grid2 = mix(grid2, targetWidth, clamp(uvDeriv * 2.0 - 1.0, 0.0, 1.0));
    grid2.x = invertLine.x ? 1.0 - grid2.x : grid2.x;
    grid2.y = invertLine.y ? 1.0 - grid2.y : grid2.y;
    return mix(grid2.x, 1.0, grid2.y);
}

void main()
{
    vec2 uv = v_Coords;
    float grid = pristineGrid(uv, width);
    vec4 bgColor = vec4(0.25f, 0.25f, 0.25f, 1.0f);
    vec4 lineColor = vec4(0.3f, 0.3f, 0.3f, 0.1f);
        
    if (uv.x > -majorAxisThickness && uv.x < majorAxisThickness)
        lineColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    if (uv.y > -majorAxisThickness && uv.y < majorAxisThickness)
        lineColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    vec4 color = mix(bgColor, lineColor, grid);
    FragColor = color;
}

