#version 460 core
layout (location = 0) out vec4 albedo;

uniform float timeInAim;
uniform float percentage;
uniform float uvMult;
uniform vec3 colour;

in vec2 texCoords;
in vec3 normal;

void main()
{
	if(dot(normal, vec3(0.0,1.0,0.0)) <= 0) discard;
	vec2 uv = texCoords;
    vec3 fragColour;
    uv.y *= uvMult;
    
    float slice;
     
    float t = timeInAim;
    if(t < 0.3)
    {
        slice = mod(uv.y,2.0f);
        if(slice < 1.0) {
            slice = 0.0;
        }
        else {
			slice = 0.5;
        }
        fragColour = colour * t/0.3;
    }
    else
    {
        float displacement = mod((t - 0.3) * 5.0, 2.0);
        slice = mod(uv.y + displacement, 2.0f);
        if(slice < 1.0) slice = 0.0;
        else slice = 0.5;
        fragColour = colour;
    }

    fragColour *= slice;

    albedo = vec4(fragColour * slice, slice);
}