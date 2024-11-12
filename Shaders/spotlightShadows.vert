#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent; // TODO: Actually use
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;

uniform mat4 vp;
uniform mat4 model;

// Animation
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMatrices[MAX_BONES];

void main()
{
    vec4 pos;

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
        pos = model * (totalPosition * vec4(aPos, 1.0));
    }
    else 
    {
        pos = model * vec4(aPos, 1.0);
    }

    gl_Position = vp * vec4(pos);
}