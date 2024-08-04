#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 VertColour;

out vec3 Colour;

uniform mat4 vp;


void main()
{
	Colour = VertColour;
	gl_Position = vp * vec4(Position, 1);
}