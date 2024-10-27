#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;

uniform vec2 offset;
uniform vec2 scale;

void main()
{
    texCoords = aTexCoords;
    gl_Position = vec4(aPos.x * scale.x + offset.x, aPos.y * scale.y + offset.y, 0.0, 1.0);
}
