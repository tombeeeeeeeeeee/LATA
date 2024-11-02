#version 460 core

in vec2 texCoords;
in vec3 lightColour;

layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 bloom;

// Normals
in vec3 fragmentNormal;

uniform float linear;
uniform float quad;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emission;

void main()
{

}