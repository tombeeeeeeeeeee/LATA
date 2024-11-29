#version 460 core

layout (location = 0) out vec4 FragColor;

uniform float lifeTime;

in vec2 TexCoord;

 struct Material {
	sampler2D albedo;
	vec3 colour;
 };

 uniform Material material;

void main()
{
	vec3 colour = texture(material.albedo, TexCoord).rgb * material.colour;
	FragColor = vec4(colour * 0.5 * min(1.0, lifeTime), 1.0);
}