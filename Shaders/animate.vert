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
out vec3 _Colour;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 vp;

//TODO: What should this actually be
const int MAX_BONES = 127;
const int MAX_BONE_PER_VERT = 4;

uniform mat4 boneMatrices[MAX_BONES];
uniform int selectedBone;

void main()
{
    test = boneWeights[0];
    test2 = boneWeights[1];
    test3 = boneWeights[2];
    fragPos = vec3(model * vec4(aPos, 1.0));
    daBoneWeights = boneWeights;

    mat4 totalPosition = mat4(0);
    for(int i = 0 ; i < MAX_BONE_PER_VERT ; i++)
    {
        if (boneIDs[i] == -1) {
            continue;
        }

        gl_Position = projection * view * model * vec4(aPos,1);
	    _Colour = vec3(0);
	    for(int i = 0; i < 4; i++)
	    {
		    if(boneIDs[i] == selectedBone)
		    {
			    _Colour = vec3(1, 1, 0);
		    }
	    }
        return;

	    //_Normal = (mMatrix * vec4(Normal,0)).xyz;
	    //_TexCoords = texCoords;
        //totalPosition += boneMatrices[boneIDs[i]] * boneWeights[i];

        
//        if(boneIDs[i] == -1) {
//            continue;
//        }
//        if(boneIDs[i] >= MAX_BONES) 
//        {
//            totalPosition = vec4(aPos,1.0f);
//            break;
//        }
//        vec4 localPosition = boneMatrices[boneIDs[i]] * vec4(aPos,1.0f);
//        totalPosition += localPosition * boneWeights[i];
    }
	
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
//    mat4 viewModel = view * model;
//    gl_Position = projection * viewModel * totalPosition;
    fragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
}