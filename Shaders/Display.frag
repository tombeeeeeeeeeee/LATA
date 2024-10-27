#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D ambientPass;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emission;
uniform sampler2D SSAO;
uniform sampler2D bloomBlur;

uniform float exposure;
uniform float bloomStrength = 0.04f;
uniform int bufferIndex;

void main()
{
    // to bloom or not to bloom
    vec3 result = vec3(0.0);
    //
    result = texture(ambientPass, texCoords).rgb;
    //
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    result = mix(result, bloomColor, bloomStrength); // linear interpolation
    vec3 em = texture(emission, texCoords).rgb;


    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);
    //
    if(em.x + em.y + em.z > 0)
    {
        result = em;
    }

    ////Gamma
    const float gamma = 2.2;

    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

    switch(bufferIndex)
    {
        //Depth
        case 0:
        //FragColor = vec4(texture(ambientPass, texCoords).rgb ,1.0);
        break;

        //Albedo
        case 1:
        FragColor = vec4(texture(albedo, texCoords).rgb, 1.0);
        break;

        //Normals
        case 2:
        FragColor = vec4(texture(normal, texCoords).rgb, 1.0);
        break;

        //Emission
        case 3:
        FragColor = vec4(texture(emission, texCoords).rgb, 1.0);
        break;

        //PBR
        case 4:
        float ro = texture(normal, texCoords).a;
        float me = texture(albedo, texCoords).a;
        float ao = texture(emission, texCoords).a;
        FragColor = vec4(ro, me, ao, 1.0);
        break;

        //SSAO
        case 5:
        FragColor = vec4(texture(SSAO, texCoords).rrr, 1.0);
        break;

        //BLOOM
        case 6:
        FragColor = vec4(texture(bloomBlur, texCoords).rgb, 1.0);
        break;
    }
}