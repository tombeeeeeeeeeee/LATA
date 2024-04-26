#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 texCoords;

// Animation
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMatrices[MAX_BONES];


void main()
{
    texCoords = aTexCoords;
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);



    vec4 pos;

    mat3 normalMatrix;

    mat4 totalPosition = mat4(0.0);
    
    if (aBoneIDs[0] != -1) {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(aBoneIDs[i] == -1) {
                continue;
            }
     
    //        vec4 localPosition = boneMatrices[aBoneIDs[i]] * vec4(aPos,1.0f);
            totalPosition += boneMatrices[aBoneIDs[i]] * aBoneWeights[i];
            vec3 localNormal = mat3(boneMatrices[aBoneIDs[i]]) * aNormal;
        }
        normalMatrix = transpose(inverse(mat3(totalPosition)));
        

        pos = lightSpaceMatrix * model * (totalPosition * vec4(aPos, 1.0));
    }
    else {
        normalMatrix = transpose(inverse(mat3(model)));
        pos = lightSpaceMatrix * vec4(vec3(model * vec4(aPos, 1.0)), 1.0);

    }
    gl_Position = pos;
}