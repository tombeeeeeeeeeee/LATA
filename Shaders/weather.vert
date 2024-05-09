#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 pos;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos.x + pos.x, aPos.y + pos.y, 0.0, 1.0);
}
