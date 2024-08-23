#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D SSAO;

uniform float exposure;
uniform float bloomStrength = 0.04f;



void main()
{
    // to bloom or not to bloom
    vec3 result = vec3(0.0);

    vec3 hdrColor = texture(scene, texCoords).rgb;
    float ambientOcclusion = 0.0;

    //vec2 texelSize = 1.0 / vec2(textureSize(SSAO, 0));
    //float ambientBlurResult = 0.0;
    //for (int x = -2; x < 2; ++x) 
    //{
    //    for (int y = -2; y < 2; ++y) 
    //    {
    //        vec2 offset = vec2(float(x), float(y)) * texelSize;
    //        ambientBlurResult += texture(SSAO, texCoords + offset).r;
    //    }
    //}
    //ambientOcclusion = ambientBlurResult / (4.0 * 4.0);
    //hdrColor *= ambientOcclusion * ambientOcclusion;

    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    result = mix(hdrColor, bloomColor, bloomStrength); // linear interpolation

    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);

    //Gamma
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}