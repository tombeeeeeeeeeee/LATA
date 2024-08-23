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

layout (location = 0) out vec4 positionColour;
layout (location = 1) out vec4 normalColour;

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

    //Catch for when NormalMap isnt set
    if(tangentNormal.r + tangentNormal.g + tangentNormal.b == 0.0)
        tangentNormal = vec3(0.5,0.5,1.0);

    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    vec3 trueNormal = TBN * tangentNormal;

    positionColour = vec4(fragmentPosInView, 1.0);
    normalColour = vec4(trueNormal, 1.0);
}

    