#version 460 core

struct Material {
    sampler2D albedo;
    sampler2D normal;
    sampler2D PBR;
    sampler2D emission;
}; 

uniform Material material;
uniform vec3 decalCentre;
uniform float depthPenertration;
uniform sampler2D depthMap;
uniform mat4 invV;
uniform mat4 invP;
uniform mat4 invM;
uniform vec2 invViewPort;

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
    vec2 screenTexCoords = gl_FragCoord.xy;
    screenTexCoords *= invViewPort;

    float sampleDepth = texture(depthMap, screenTexCoords).r;
    if(abs(sampleDepth - gl_FragCoord.z) > depthPenertration) discard;

    vec4 NDC = vec4(screenTexCoords * 2.0 - 1.0, sampleDepth * 2.0 - 1.0, 1.0);
    vec4 clipPos = invP * NDC;
    vec3 screenPosition = clipPos.xyz / clipPos.w;
    vec3 worldPos = (invV * vec4(screenPosition, 1.0)).xyz;

    vec4 objPos = invM * vec4(worldPos, 1.0);


    vec2 decalTexCoords = texCoords;

    TBN = mat3(normalize(fragmentTangent), -normalize(fragmentBitangent), -normalize(fragmentNormal));
    vec3 tangentNormal = texture(material.normal, decalTexCoords).rgb;

    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    vec3 trueNormal = TBN * tangentNormal;
    vec4 trueAlbedo = texture(material.albedo, decalTexCoords);
    vec3 albedo3 = trueAlbedo.rgb * fragmentColour;
    vec3 PBR = texture(material.PBR, decalTexCoords).rgb;
    vec3 emission3 = texture(material.emission, decalTexCoords).rgb;

    //Albedo packed with PBR metallic
    albedo = vec4(albedo3, trueAlbedo.a);

    //Normal packed with PBR Roughness
    normal = vec4(trueNormal, texture(material.normal, decalTexCoords).a);

    //Emission packed with PBR AO
    emission = vec4(emission3, 1.0);

    pbr = vec4(PBR, 1.0);
}

    