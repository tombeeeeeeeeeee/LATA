#version 460 core
struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    float shininess;
}; 
out vec4 FragColor;

in vec2 TexCoord;

uniform Material material;

void main()
{
	FragColor = texture(material.diffuse1, TexCoord);
    //FragColor = vec4(TexCoord, 1, 1);
}