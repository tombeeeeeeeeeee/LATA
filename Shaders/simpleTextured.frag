#version 460 core

layout (location = 1) out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D albedo;
};

uniform Material material;

void main()
{
	FragColor = texture(material.albedo, TexCoord);
}