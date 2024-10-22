#version 460 core

struct Material {
    sampler2D albedo;
    sampler2D normal;
    sampler2D PBR;
    sampler2D emission;
}; 

uniform Material material;

in vec2 texCoords;
in vec3 fragmentPos;
in vec3 fragmentColour;
in vec3 fragmentPosInView;

layout (location = 0) out vec4 normal;
layout (location = 1) out vec4 albedo;
layout (location = 2) out vec4 emission;

// Lighting
#define PI 3.1415926535

vec3 viewDir;

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
    vec3 albedo = 
    colourColour =  texture(material.albedo, texCoords);
    emissionColour =  texture(material.emission, texCoords);
    normalColour = vec4(trueNormal, 1.0);
}

    