#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;

out vec2 TexCoord;
out vec4 firstBoneID;

uniform mat4 model;
uniform mat4 vp;

void main()
{
	TexCoord = aTexCoord;
	firstBoneID = boneIDs;
	gl_Position = vp * vec4(vec3(model * vec4(aPos, 1.0)), 1.0);
}


//
//#version 430 core
//
//layout(location = 0) in vec3 pos;
//layout(location = 1) in vec3 norm;
//layout(location = 2) in vec2 tex;
//layout(location = 5) in ivec4 boneIds; 
//layout(location = 6) in vec4 weights;
//	
//uniform mat4 projection;
//uniform mat4 view;
//uniform mat4 model;
//	
//const int MAX_BONES = 100;
//const int MAX_BONE_INFLUENCE = 4;
//uniform mat4 finalBonesMatrices[MAX_BONES];
//	
//out vec2 TexCoords;
//	
//void main()
//{
//    vec4 totalPosition = vec4(0.0f);
//    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
//    {
//        if(boneIds[i] == -1) 
//            continue;
//        if(boneIds[i] >=MAX_BONES) 
//        {
//            totalPosition = vec4(pos,1.0f);
//            break;
//        }
//        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
//        totalPosition += localPosition * weights[i];
//        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
//    }
//		
//    mat4 viewModel = view * model;
//    gl_Position =  projection * viewModel * totalPosition;
//    TexCoords = tex;
//}