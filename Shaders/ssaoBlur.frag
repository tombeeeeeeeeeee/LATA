#version 460 core

uniform sampler2D SSAO;

in vec2 texCoords;
layout (location = 0) out float FragColor;

void main() 
{
    float ambientOcclusion = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(SSAO, 0));
    float ambientBlurResult = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            ambientBlurResult += texture(SSAO, texCoords + offset).r;
        }
    }
    ambientOcclusion = ambientBlurResult / (4.0 * 4.0);
    FragColor = ambientOcclusion;
}