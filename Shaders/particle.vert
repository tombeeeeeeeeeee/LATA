#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;



out vec2 TexCoord;

uniform mat4 model;
uniform mat4 vp;
uniform vec3 positions[1000];

void main()
{
	TexCoord = aTexCoord;
	vec3 offset = positions[gl_InstanceID];
	gl_Position = vp * vec4(vec3(model * vec4(aPos + offset, 1.0)), 1.0);
}
