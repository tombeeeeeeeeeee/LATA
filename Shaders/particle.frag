#version 460 core

layout (location = 1) out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D albedo;
};

uniform Material material;

void main()
{
	FragColor = vec4(1, 0, 1, 1);
	//FragColor = texture(material.albedo, TexCoord);
}