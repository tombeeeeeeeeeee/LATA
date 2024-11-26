#version 460 core
layout (location = 0) out vec4 albedo;

uniform float timeInAim;
uniform float percentage;
uniform float uvMult;
uniform vec3 colour;

in vec2 texCoords;

void main()
{
	vec2 uv = texCoords;
    uv.x *= 2.0;
    uv.x -= 1.0;
    uv.y *= uvMult;
    uv.x *= 5.0;

	float t = pow(percentage, 4.0);
    float focusLevel = 1.0 - t;
    focusLevel *= focusLevel;
 
    float curveHeight = (1.0 - focusLevel) * (sin(uv.y * 2.0 * 3.14159) + sin(uv.y * 2.0 * 3.14159 * 1.7) * 0.3);
    float intensity = cos((uv.x - curveHeight) * 0.5);

    intensity = pow(intensity, pow(focusLevel, 6.0) * 250.0f) * pow(focusLevel, 3.0);
 
    albedo = vec4(colour * intensity * 3.0 * (1.0 - t * t), (1.0 - t) * (1.0 - t) * intensity * pow(abs(uv.x * 0.5), percentage) * 4.0);
    albedo = max(albedo, 0.0);
}