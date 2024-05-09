#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec2 wind;

void main()
{
    //vec3 colour = texture(image, TexCoord).rgb;
    
    
    vec3 colour;
    //colour = vec3(1, 0, 1);
    colour.x = wind.x / 2 + 0.5;
    colour.y = wind.y / 2 + 0.5;
    colour.z = -wind.x / 2 + 0.5;

    FragColor = vec4(colour, 1.0);
}

