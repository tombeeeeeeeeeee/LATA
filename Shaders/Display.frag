#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D colour;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emission;
uniform sampler2D pbr;
uniform sampler2D SSAO;
uniform sampler2D bloomBlur;
uniform sampler2D lightBuffer;

uniform float exposure;
uniform float bloomStrength = 0.04f;
uniform int bufferIndex;

void main()
{
    // to bloom or not to bloom
    vec3 result = vec3(0.0);
    //
    result = texture(colour, texCoords).rgb;
    //
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    result = mix(result, bloomColor, bloomStrength); // linear interpolation


    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);


    ////Gamma
    const float gamma = 2.2;

    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

    switch(bufferIndex)
    {
        //Depth
        case 0:
        //FragColor = vec4(texture(lightBuffer, texCoords).rgb ,1.0);
        break;

        //Albedo
        case 1:
        FragColor = vec4(texture(albedo, texCoords).rgb, 1.0);
        break;

        //Normals
        case 2:
        FragColor = vec4((texture(normal, texCoords).rgb + 1.0) / 2.0, 1.0);
        break;

        //Emission
        case 3:
        FragColor = vec4(texture(emission, texCoords).rgb, 1.0);
        break;

        //PBR
        case 4:
        FragColor =  vec4(texture(pbr, texCoords).rgb, 1.0);

        break;

        // Roughness
        case 5:
        float roughness = texture(pbr, texCoords).g;
        FragColor = vec4(roughness.rrr, 1.0);
        break;
        
        // Metallic
        case 6:
        float metallic = texture(pbr, texCoords).r;
        FragColor = vec4(metallic.xxx, 1.0);
        break;
        
        // AO
        case 7:
        float ao =  texture(pbr, texCoords).b;
        FragColor = vec4(ao.xxx, 1.0);
        break;

        //SSAO
        case 8:
        FragColor = vec4(texture(SSAO, texCoords).rrr, 1.0);
        break;

        //BLOOM
        case 9:
        FragColor = vec4(texture(bloomBlur, texCoords).rgb, 1.0);
        break;
    }
}