#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform float exposure;
uniform float bloomStrength = 0.04f;


void main()
{
    // to bloom or not to bloom
    vec3 result = vec3(0.0);

    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    result = mix(hdrColor, bloomColor, bloomStrength); // linear interpolation

    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);

    //Gamma
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}