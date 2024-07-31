#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent; // TODO: Actually use
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;

struct DirectionalLight {
    vec3 direction;
    vec3 colour;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 colour;
};

struct Spotlight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 colour;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};


// General
uniform mat4 vp;
uniform mat4 model;
uniform vec3 materialColour;

out vec3 fragmentColour;
out vec2 texCoords;
out vec3 fragPos;

// Animation
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMatrices[MAX_BONES];

// Lighting, normal mapping
#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Spotlight spotlight;
uniform vec3 viewPos;

out vec3 tangentViewPos;
out vec3 tangentFragPos;
out vec3 tangentSpotlightPos;
out vec3 tangentPointLightsPos[MAX_POINT_LIGHTS];
out mat3 inverseTBN;

// Shadows
uniform mat4 directionalLightSpaceMatrix;

out vec3 normal;
out vec3 fragNormal;
out vec3 fragTan;
out vec3 fragBi;
out vec4 directionalLightSpaceFragPos;

// Main
void main()
{
    normal = aNormal;
    texCoords = aTexCoords;
    vec4 pos;

    // Normal calculations
    mat3 normalMatrix;
    
    
    // Animation

    if (aBoneIDs[0] != -1) {
        mat4 totalPosition = mat4(0.0);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(aBoneIDs[i] == -1) {
                continue;
            }     
            totalPosition += boneMatrices[aBoneIDs[i]] * aBoneWeights[i];
            vec3 localNormal = mat3(boneMatrices[aBoneIDs[i]]) * aNormal;
        }
        normalMatrix = transpose(inverse(mat3(totalPosition)));
        fragPos = vec3(model * (totalPosition * vec4(aPos, 1.0)));
    }
    else {
        normalMatrix = transpose(inverse(mat3(model)));
        fragPos = vec3(model * vec4(aPos, 1.0));
    }
    directionalLightSpaceFragPos = directionalLightSpaceMatrix * vec4(fragPos, 1.0); // frag is also timesed by model

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);
    
    // The transpose of an orthogonal matrix (each axis is a perpendicular unit vector) equals its inverse
    inverseTBN = mat3(T, B, N);
    mat3 TBN = transpose(inverseTBN);

    fragNormal = normalize((model * vec4(aNormal, 0.0)).xyz);
    fragTan = normalize((model * vec4(aTangent, 0.0)).xyz);
    fragTan = normalize(fragTan - dot(fragTan, fragNormal) * fragNormal);
    fragBi = cross(fragNormal, fragTan);

    tangentSpotlightPos = TBN * spotlight.position;

    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        tangentPointLightsPos[i] = TBN * pointLights[i].position;
    }    
    
    tangentViewPos = TBN * viewPos;
    tangentFragPos = TBN * fragPos;
		
    gl_Position = vp * vec4(fragPos, 1.0);
    fragmentColour = materialColour;
}