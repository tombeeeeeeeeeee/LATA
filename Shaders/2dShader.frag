#version 460 core
layout (location = 0) out vec4 FragColor;

uniform sampler2D albedo;

in vec3 FragPos;
in vec2 TexCoords;

void main()
{
    vec3 albedoColour = texture(albedo, TexCoords).rgb;

    FragColor = vec4(albedoColour, 1.0);

    //FragColor = vec4(1.0.xxxx);
} 
