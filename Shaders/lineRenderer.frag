#version 460 core

out vec4 FragColour;

in vec3 Colour;

void main()
{
	FragColour = vec4(Colour, 1);
}