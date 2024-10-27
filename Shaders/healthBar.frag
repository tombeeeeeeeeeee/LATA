#version 460 core

in vec2 texCoords;

out vec4 FragColour;

uniform float healthPercent;
uniform vec3 healthColour;
uniform vec3 backgroundColour;

void main()
{
	if (texCoords.x < healthPercent) {
		FragColour = vec4(healthColour, 1.0);
	}
	else {
		FragColour = vec4(backgroundColour, 1.0);
	}
}