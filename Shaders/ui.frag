#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

void main()
{
    vec3 col = texture(image, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
} 