#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 vp;

void main()
{
    //FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;

    vec4 temp = vp * vec4(aPos, 0.0, 1.0);
    gl_Position = vec4(temp.x, temp.y, 0.0, 1.0);
}