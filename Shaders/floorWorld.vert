#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;


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

out vec3 fragmentNormal;
out vec3 fragmentTangent;
out vec3 fragmentBitangent;

uniform vec2 worldMin;
uniform vec2 worldMax;


void main()
{
    vec4 pos;

    pos = model * vec4(aPos, 1.0);

    fragmentNormal = normalize((model * vec4(aNormal, 0.0)).xyz);
    fragmentTangent = normalize((model * vec4(aTangent, 0.0)).xyz);
    fragmentTangent = normalize(fragmentTangent - dot(fragmentTangent, fragmentNormal) * fragmentNormal);
    fragmentBitangent = cross(fragmentNormal,  fragmentTangent);

    gl_Position = vp * pos;
    ndc = gl_Position;
    fragmentColour = materialColour;
    //texCoords = (pos.xz - worldMin) / worldMax;
    texCoords = vec2((pos.xz - worldMin) / (worldMax - worldMin));
    texCoords.y = 1 - texCoords.y;
}