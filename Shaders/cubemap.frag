#version 460 core
layout (location = 0) out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube cubeMap;

void main()
{
	FragColor = texture(cubeMap, TexCoord);
}