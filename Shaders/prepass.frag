#version 460 core

struct Material {
    sampler2D albedo;
    sampler2D normal;
    sampler2D PBR;
    sampler2D emission;
}; 

uniform Material material;

in vec2 texCoords;
in vec3 fragmentColour;

layout (location = 0) out vec4 normal;
layout (location = 1) out vec4 albedo;
layout (location = 2) out vec4 emission;
layout (location = 3) out vec4 pbr;

// Normals
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;
mat3 TBN;

void main()
{
    TBN = mat3(normalize(fragmentTangent), normalize(fragmentBitangent), normalize(fragmentNormal));
    vec3 tangentNormal = texture(material.normal, texCoords).rgb;

    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    vec3 trueNormal = TBN * tangentNormal;

    vec3 albedo3 = texture(material.albedo, texCoords).rgb * fragmentColour;
    
    vec3 PBR = texture(material.PBR, texCoords).rgb;
    //vec2 mipAccessed = textureQueryLod(material.albedo, texCoords);
    //
    //float mipValue = max(mipAccessed.x, mipAccessed.y);
    //
    //vec3 debugColours[6] = {vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)};
    //
    //float fractional = fract(mipValue);
    //
    //
    //vec3 lower = debugColours[int(mipValue)];
    //vec3 upper = debugColours[min(int(mipValue) + 1, 5)];
    //vec3 debugColour = mix(lower, upper, fractional);
    ////if (mipValue < 1.0) debugColour = vec3(0.5, 0.0, 0.5);
    //if (mipValue < 1.0) debugColour = vec3(1.0, 0.0, 1.0);
    //albedo3 = debugColour;
    
    vec3 emission3 = texture(material.emission, texCoords).rgb;

    //Albedo packed with PBR metallic
    albedo = vec4(albedo3, 1.0);

    //Normal packed with PBR Roughness
    normal = vec4(trueNormal, 1.0);

    //Emission packed with PBR AO
    emission = vec4(emission3, 1.0);

    pbr = vec4(PBR, 1.0);
}

    