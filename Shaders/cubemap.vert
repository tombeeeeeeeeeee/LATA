#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 vp; // View Projection

void main()
{
	TexCoord = aPos;
	vec4 pos = vp * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}
