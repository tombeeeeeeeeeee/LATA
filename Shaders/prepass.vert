#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent; // TODO: Actually use
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;

// General
uniform mat4 vp;
uniform mat4 model;
uniform mat4 view;
uniform vec3 materialColour;

out vec3 fragmentColour;
out vec2 texCoords;
out vec2 screenPosition;

// Animation
const int MAX_BONES = 130;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMatrices[MAX_BONES];

out vec3 fragmentNormal;
out vec3 fragmentTangent;
out vec3 fragmentBitangent;

void main()
{
    texCoords = aTexCoords;
    vec4 pos;

    // Animation
    if (aBoneIDs[0] != -1) {
        mat4 averagedTransform = mat4(0.0);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(aBoneIDs[i] == -1) {
                continue;
            }     
            averagedTransform += boneMatrices[aBoneIDs[i]] * aBoneWeights[i];
            //vec3 localNormal = mat3(boneMatrices[aBoneIDs[i]]) * aNormal;
        }
        pos = model * (averagedTransform * vec4(aPos, 1.0));
        fragmentNormal = normalize((model * averagedTransform * vec4(aNormal, 0.0)).xyz);
        fragmentTangent = normalize((model * averagedTransform * vec4(aTangent, 0.0)).xyz);
    }
    else 
    {
        pos = model * vec4(aPos, 1.0);
        fragmentNormal = normalize((model * vec4(aNormal, 0.0)).xyz);
        fragmentTangent = normalize((model * vec4(aTangent, 0.0)).xyz);
    }

    
    fragmentTangent = normalize(fragmentTangent - dot(fragmentTangent, fragmentNormal) * fragmentNormal);
    fragmentBitangent = cross(fragmentNormal, fragmentTangent);

    gl_Position = vp * vec4(pos);
    screenPosition = (gl_Position.xyz / gl_Position.w).xy;
    screenPosition = screenPosition * 0.5 + 0.5;
    fragmentColour = materialColour;
}