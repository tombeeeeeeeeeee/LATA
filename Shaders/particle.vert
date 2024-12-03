#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 vp;

struct Particle {
	vec4 pos;
};

layout(std430, binding = 3) buffer Particles
{
    Particle data[];
};

void main()
{	
	TexCoord = aTexCoord;
	mat4 scale = mat4(0.0);
	scale[0][0] = data[gl_InstanceID].pos.w;
	scale[1][1] = data[gl_InstanceID].pos.w;
	scale[2][2] = data[gl_InstanceID].pos.w;
	scale[3][3] = 1.0;
	gl_Position = vp * vec4(vec3(scale * model * vec4(aPos, 1.0)) + data[gl_InstanceID].pos.rgb, 1.0);
	
}
