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

out vec2 texCoords;

void main()
{
    texCoords = aTexCoords;
    vec4 pos;
    pos = model * vec4(aPos, 1.0);
    gl_Position = vp * vec4(pos);
}