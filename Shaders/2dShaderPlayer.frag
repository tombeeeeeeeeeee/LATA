#version 460 core
//layout (location = 0) out vec4 FragColor;

uniform sampler2D tex;

in vec3 FragPos;
in vec2 TexCoords;

layout (location = 0) out vec4 normal;
layout (location = 1) out vec4 albedo;
layout (location = 2) out vec4 emission;
layout (location = 3) out vec4 pbr;

struct PlayerInfo
{
	vec2 pos;
	vec2 vel;
};

const float playerRenderRadius = 4.0;

layout(std430, binding = 2) buffer PlayerSlot
{
	PlayerInfo playerInfo;
};



void main()
{
    if (length(FragPos.xy - playerInfo.pos) > playerRenderRadius)
    {
        discard;
    }
    vec3 albedoColour = vec3(1, 0.3, 0.1);
//    vec3 albedoColour = texture(tex, TexCoords).rgb;

    albedo = vec4(albedoColour, 1.0);
    pbr = vec4(0, 0, 0, 1);
    emission = vec4(0.0, 0.0, 0.0, 0.0);
    normal = vec4(0, 0, 1, 0);


    //FragColor = vec4(1.0.xxxx);
} 
