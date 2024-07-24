#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 vp;

void main()
{
    WorldPos = aPos;  
    gl_Position =  vp * vec4(WorldPos, 1.0);
}