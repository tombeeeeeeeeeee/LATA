#version 460 core

layout (location = 0) out vec4 FragColor;

uniform float lifeTime;
uniform float lifeSpan;

in vec2 TexCoord;

 struct Material {
	sampler2D albedo;
	vec3 colour;
 };

 uniform Material material;
 uniform sampler2D colourOverTime;

void main()
{
	vec3 colour = texture(material.albedo, TexCoord).rgb * material.colour;
	colour *= texture(colourOverTime, vec2(lifeTime/lifeSpan, 0.0)).rgb;
	FragColor = vec4(colour * 0.5 * min(1.0, lifeTime), 1.0);
}