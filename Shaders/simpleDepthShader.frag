#version 460 core

const float alphaDiscardBelowLevel = 0.01;

uniform sampler2D alphaDiscardMap;

in vec2 texCoords;

void main()
{
	float alpha = texture(alphaDiscardMap, texCoords).a;
	if (alpha < alphaDiscardBelowLevel) {
		discard;
	}

	//gl_FragDepth = gl_FragCoord.z; // Shouldn't need to have this line anyway
}