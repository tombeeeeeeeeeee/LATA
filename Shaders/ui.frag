#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image;

void main()
{
    vec3 colour = texture(image, TexCoord).rgb;
    FragColor = vec4(colour, 1.0);
}