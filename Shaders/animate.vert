#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;

out vec2 TexCoord;
out vec4 daBoneWeights;
out float test;
out float test2;
out float test3;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 vp;

//TODO: What should this actually be
const int MAX_BONES = 100;
const int MAX_BONE_PER_VERT = 4;

uniform mat4 boneMatrices[MAX_BONES];
uniform int selectedBone;

void main()
{
    vec4 totalPosition = { 0, 0, 0, 0 };
    for(int i = 0 ; i < MAX_BONE_PER_VERT ; i++)
    {
        if(boneIDs[i] == -1) 
            continue;
        if(boneIDs[i] >=MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        vec4 localPosition = boneMatrices[boneIDs[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * boneWeights[i];
        vec3 localNormal = mat3(boneMatrices[boneIDs[i]]) * aNormal;
    }
		
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
    TexCoord = aTexCoord;

//    mat3 normalMatrix = transpose(inverse(mat3(BoneTransform)));
//vec3 T = normalize(normalMatrix * aTangent);
//vec3 B = normalize(normalMatrix * aBitangent);
//vec3 N = normalize(normalMatrix * aNormal);
//
//vs_out.TBN		=	mat3(T, B, N);
//vs_out.FragPos		=	vec3(model * totalPosition);
//vs_out.Normal		=	N;
//vs_out.TexCoords	=	aTexCoords;

    fragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
}