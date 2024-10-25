#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
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
uniform mat4 view;
uniform vec3 materialColour;


out vec3 fragmentColour;
out vec2 texCoords;
out vec4 ndc;

// Animation
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMatrices[MAX_BONES];

// Lighting, normal mapping
#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Spotlight spotlight;
uniform vec3 viewPos;
out vec3 spotlightPos;
out vec3 pointLightsPos[MAX_POINT_LIGHTS];

out vec3 fragmentPosInView;
out vec3 fragmentPos;
out vec4 directionalLightSpaceFragPos;

// Shadows
uniform mat4 directionalLightSpaceMatrix;

out vec3 fragmentNormal;
out vec3 fragmentTangent;
out vec3 fragmentBitangent;


uniform vec2 worldMin;
uniform vec2 worldMax;


uniform mat4 lightSpaceMatrix;
//uniform mat4 model;

//out vec2 texCoords;


void main()
{
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);


    vec4 pos;

    pos = model * vec4(aPos, 1.0);

    directionalLightSpaceFragPos = directionalLightSpaceMatrix * vec4(fragmentPos, 1.0); // frag is also timesed by model

    fragmentNormal = normalize((model * vec4(aNormal, 0.0)).xyz);
    fragmentTangent = normalize((model * vec4(aTangent, 0.0)).xyz);
    fragmentTangent = normalize(fragmentTangent - dot(fragmentTangent, fragmentNormal) * fragmentNormal);
    fragmentBitangent = cross(fragmentNormal,  fragmentTangent);

    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        pointLightsPos[i] = pointLights[i].position;
    }    

    gl_Position = vp * pos;
    ndc = gl_Position;
    fragmentColour = materialColour;
    //texCoords = (pos.xz - worldMin) / worldMax;
    texCoords = vec2((pos.xz - worldMin) / (worldMax - worldMin));
    texCoords.y = 1 - texCoords.y;

    fragmentPosInView = (view * pos).xyz;
}