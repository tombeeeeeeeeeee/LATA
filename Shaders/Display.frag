#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depth;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emission;
uniform sampler2D SSAO;
uniform sampler2D bloomBlur;

uniform float exposure;
uniform float bloomStrength = 0.04f;
uniform int buffer;

void main()
{
    // to bloom or not to bloom
    vec3 result = vec3(0.0);

    vec3 hdrColor = texture(scene, texCoords).rgb;

    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    result = mix(hdrColor, bloomColor, bloomStrength); // linear interpolation

    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);

    //Gamma
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
    switch(buffer)
    {
        //Depth
        case 0:
        float d = texture(depth, texCoords).r;
        result = vec4(d, d, d,1.0);
        break;

        //Albedo
        case 1:
        result = vec4(texture(albedo, texCoords).rgb, 1.0);
        break;

        //Normals
        case 2:
        result = vec4(texture(normal, texCoords).rgb, 1.0);
        break;

        //Emission
        case 3:
        result = vec4(texture(emission, texCoords).rgb, 1.0);
        break;

        //PBR
        case 4:
        float ro = texture(normal, texCoords).a;
        float me = texture(albedo, texCoords).a;
        float ao = texture(emission, texCoords).a;
        result = vec4(ro, me, ao, 1.0);
        break;

        //SSAO
        case 5:
        result = vec4(texture(SSAO, texCoords).rgb, 1.0);
        break;

        //BLOOM
        case 5:
        result = vec4(texture(bloomBlur, texCoords).rgb, 1.0);
        break;
    }
}