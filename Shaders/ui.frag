#version 460 core

out vec4 FragColor;

in vec2 TexCoord;


struct Material {
    sampler2D albedo;
};

uniform Material material;

void main()
{
    vec3 colour = texture(material.albedo, TexCoord).rgb;
    //FragColor = vec4(TexCoord.xy, 0, 1);
    FragColor = vec4(colour, 1.0);
}