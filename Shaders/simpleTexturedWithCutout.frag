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
	
	vec3 colour3 = pow(colour.rgb, vec3(1/2.2));
	
	FragColor = vec4(colour3, 1.0);
}