#version 330

layout (location = 0) in vec3 vertPosCoords;
layout (location = 1) in vec4 vertRgbaColor;
layout (location = 2) in int biomeIndex;
layout (location = 3) in int plateIndex;

out vec4 rgbaColor;

uniform vec4 BiomeColors[64];
uniform vec4 PlateColors[64];

uniform mat4 MVP;
uniform int Focus;

void main()
{
    if (Focus == 1) rgbaColor = BiomeColors[biomeIndex];
    else if (Focus == 2) rgbaColor = PlateColors[plateIndex];
    else rgbaColor = vertRgbaColor;
    
    gl_Position = MVP * vec4(vertPosCoords, 1.0);
}
