#version 460 core

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 bloomColour;

in vec3 Colour;

void main()
{
	FragColour = vec4(Colour, 1);
	bloomColour = FragColour;
}