#version 460 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 bloomColour;

uniform vec3 materialColour;

void main()
{
    FragColor = vec4(materialColour, 1.0); // set all 4 vector values to 1.0
    bloomColour = FragColor;
}