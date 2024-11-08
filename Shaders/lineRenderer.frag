#version 460 core

layout (location = 0) out vec4 albedo;

in vec3 Colour;

void main()
{
	albedo = vec4(Colour, 1);
}