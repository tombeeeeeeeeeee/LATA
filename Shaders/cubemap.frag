#version 460 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 bloomColour;
in vec3 TexCoord;

uniform samplerCube cubeMap;

void main()
{
	FragColor = texture(cubeMap, TexCoord);
	if(dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722)) > 0.5)
    {
        bloomColour = FragColor;
    }
    else
    {
        bloomColour = vec4(0.0,0.0,0.0,1.0);
    }
}