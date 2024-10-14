#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 3) in vec3 aOffset;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 vp;

void main()
{	
	TexCoord = aTexCoord;
	gl_Position = vp * vec4(vec3(model * vec4(aPos, 1.0)) + aOffset, 1.0);
	
}
