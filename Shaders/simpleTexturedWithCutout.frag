#version 460 core

out vec4 FragColor;

in vec2 texCoords;

struct Material {
	sampler2D albedo;
	float alpha;
};

uniform Material material;


// TODO: Rename file
void main()
{
	vec4 colour = texture(material.albedo, texCoords).rgba;
	//if (colour.a != 1) {
	//	discard;
	//	return;
	//}
	
	
	vec3 colour3 = pow(colour.rgb * colour.a, vec3(1/2.2));
	
	FragColor = vec4(colour3, colour.a * material.alpha);
}