#version 460 core

layout (location = 0) out vec4 FragColor;


in vec2 TexCoord;

 struct Material {
	sampler2D albedo;
	vec3 colour;
 };

 uniform Material material;

void main()
{
	float fakeAlpha = 0.01;
	vec3 colour = texture(material.albedo, TexCoord).rgb * material.colour;
	FragColor = vec4(colour * fakeAlpha, 1);
}