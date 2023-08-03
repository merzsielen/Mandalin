#version 330

layout (location = 0) in vec3 vertPosCoords;
layout (location = 1) in vec4 vertRgbaColor;
layout (location = 2) in int biomeIndex;
layout (location = 3) in int plateIndex;
layout (location = 4) in float temperature;
layout (location = 5) in float rainfall;
layout (location = 6) in int populationIndex;
layout (location = 7) in int languageIndex;

out vec4 rgbaColor;

uniform vec4 BiomeColors[64];
uniform vec4 PlateColors[64];

uniform mat4 MVP;
uniform int Focus;

const vec4 lowestRain = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 midRain = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 highestRain = vec4(0.0, 1.0, 0.5, 1.0);

const vec4 lowestTemp = vec4(0.9, 1.0, 1.0, 1.0);
const vec4 midTemp = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 highestTemp = vec4(1.0, 0.0, 0.0, 1.0);

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

void main()
{
    if (Focus == 0) rgbaColor = vertRgbaColor;
    else if (biomeIndex == 0 || Focus == 1) rgbaColor = BiomeColors[biomeIndex];
    else if (Focus == 2) rgbaColor = PlateColors[plateIndex];
    else if (Focus == 3)
    {
        vec4 midA = mix(lowestTemp, midTemp, temperature);
        vec4 midB = mix(midTemp, highestTemp, temperature);
        rgbaColor = mix(midA, midB, temperature);
    }
    else if (Focus == 4)
    {
        vec4 midA = mix(lowestRain, midRain, rainfall);
        vec4 midB = mix(midRain, highestRain, rainfall);
        rgbaColor = mix(midA, midB, rainfall);
    }
    else if (Focus == 5 && populationIndex > 0)
    {
        float r = mod(rand(populationIndex), 1.0);
        float g = mod(rand(populationIndex + 1), 1.0);
        float b = mod(rand(populationIndex + 2), 1.0);
        
        rgbaColor = vec4(r, g, b, 1.0);
    }
    else if (Focus == 5)
    {
        rgbaColor = vertRgbaColor;
    }
    else if (Focus == 6 && languageIndex > 0)
    {
        float r = mod(rand(languageIndex), 1.0);
        float g = mod(rand(languageIndex + 1), 1.0);
        float b = mod(rand(languageIndex + 2), 1.0);

        rgbaColor = vec4(r, g, b, 1.0);
    }
    else if (Focus == 6)
    {
        rgbaColor = vertRgbaColor;
    }
    
    gl_Position = MVP * vec4(vertPosCoords, 1.0);
}
