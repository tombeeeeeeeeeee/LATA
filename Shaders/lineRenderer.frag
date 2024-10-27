#version 460 core

layout (location = 0) out vec4 normal;
layout (location = 1) out vec4 albedo;
layout (location = 2) out vec4 emission;
layout (location = 3) out vec4 bloom;

in vec3 Colour;

void main()
{
	normal = vec4(0.0,1.0,0.0,0.0);
	albedo = vec4(0);
	emission = vec4(Colour, 1);
	bloom = vec4(Colour * 10, 1);
}