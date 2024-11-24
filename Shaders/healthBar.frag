#version 460 core

in vec2 texCoords;

out vec4 FragColour;

uniform float healthPercent;
uniform vec3 healthColour;
uniform vec3 backgroundColour;
uniform bool horizontal;

void main()
{
	bool check;
	if (horizontal) {
		check = texCoords.x < healthPercent;
	}
	else {
		check = texCoords.y < healthPercent;
	}
	if (check) {
		FragColour = vec4(healthColour, 1.0);
	}
	else {
		FragColour = vec4(backgroundColour, 1.0);
	}
}