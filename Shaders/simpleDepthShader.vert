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

void main()
{
    texCoords = aTexCoords;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}