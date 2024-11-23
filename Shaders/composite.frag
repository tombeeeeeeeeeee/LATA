#version 460 core

layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 bloom;

in vec2 texCoords;

uniform sampler2D lines;
uniform sampler2D lights;

void main()
{
	vec4 lineColour = texture(lines, texCoords).rgba;
	vec3 lightsColour = texture(lights, texCoords).rgb;
	colour = vec4(lineColour.rgb * lineColour.a + lightsColour, 1.0);
	vec4 lightsBloom;
	float lightsBrightness = dot(lightsColour, vec3(0.2126, 0.7152, 0.0722));
	if(lightsBrightness > 1)
	{ 
		lightsBloom = vec4(lightsColour, min(lightsBrightness - 1.0, 1.0));
	}
	else 
    {
        lightsBloom = vec4(0.0, 0.0, 0.0, 1.0);
    }
	bloom = vec4(lightsBloom + 5.0 * vec4(lineColour.rgb * lineColour.a, 1.0));
}