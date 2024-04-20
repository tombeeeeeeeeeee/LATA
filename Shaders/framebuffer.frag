#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
    vec3 colour = texture(screenTexture, TexCoord).rgb;
    FragColor = vec4(colour, 1.0);
//    FragColor = vec4(vec3(colour.rg + (TexCoord.rg / 2), colour.b), 1.0);

    //FragColor = vec4((colour.r + colour.g + colour.b).rrr / 3.0f, 1.0);
    //FragColor = vec4(-colour + 1.0f, 1.0f);

}