#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depth;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emission;
uniform sampler2D SSAO;

uniform samplerCube skybox;
uniform samplerCube irradiance;
uniform samplerCube prefilterMap;	
uniform sampler2D brdfLUT;
vec3 specularIBL(vec3 trueNormal, vec3 viewDirection);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

main
{
}

