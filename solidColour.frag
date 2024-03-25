#version 460 core
out vec4 FragColor;

uniform vec3 colour;

void main()
{
    FragColor = vec4(colour, 1.0); // set all 4 vector values to 1.0
}