#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;

uniform vec2 camPos;
uniform vec2 camSinCos;
uniform float aspectRatio;
uniform float orthScale;

void main()
{
	vec2 pos = aPos * 0.5; // Should probably just be rendering with a different size quad
	pos.x *= aspectRatio;
	pos *= orthScale;

	FragPos = vec3(
					camPos.x - (pos.x * camSinCos.y - pos.y * camSinCos.x),
					camPos.y - (pos.y * camSinCos.y + pos.x * camSinCos.x),
					0.0
	);

	TexCoords = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}