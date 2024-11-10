#version 460 core

out vec4 FragColor;

in vec2 texCoords;

struct Material {
	sampler2D albedo;
};

uniform Material material;

void main()
{
	vec4 colour = texture(material.albedo, texCoords).rgba;
	if (colour.a != 1) {
		discard;
		return;
	}
	FragColor = colour;
}