#version 460 core

layout (location = 0) out vec4 FragColour;
in vec2 texCoords;

uniform sampler2D screenDepth;
uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenEmission;
uniform sampler2D screenSSAO;

uniform samplerCube skybox;
uniform sampler2D roomLight;

uniform mat4 invP;
uniform mat4 invV;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 camPos;
uniform vec2 mapMins;
uniform vec2 mapDimensions;
uniform float ambientIntensity;


vec3 trueAlbedo;
float roughness;
float metallic;
float ao;
vec3 screenPos;
vec3 fragPos;

vec3 viewDirection;
vec3 F0;

float DistributionGGX(vec3 N, vec3 H);
float GeometrySchlickGGX(float NdotV);
float GeometrySmith(vec3 N, vec3 V, vec3 L);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 Radiance(
    vec3 normal
);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
#define PI 3.1415926535
const float MAX_REFLECTION_LOD = 4.0;

void main()
{
	float depthValue = texture(screenDepth, texCoords).r;

    if(depthValue == 1.0)
    {
        FragColour = vec4(0.0);
        return;
    }

    vec4 NDC = vec4(texCoords * 2.0 - 1.0, depthValue * 2.0 - 1.0, 1.0);
    vec4 clipPos = invP * NDC;
    screenPos = clipPos.xyz / clipPos.w;
    fragPos = (invV * vec4(screenPos, 1.0)).xyz;


    viewDirection = normalize(camPos - fragPos);

    vec4 emission = texture(screenEmission, texCoords);
    vec4 albedo = texture(screenAlbedo, texCoords);
    vec4 normal = texture(screenNormal, texCoords);
    vec3 trueNormal = normal.rgb;
    trueAlbedo = albedo.rgb;
    roughness = normal.a;
    metallic = albedo.a;
    ao = emission.a;
    if(ao == 0) ao = 1.0;

    vec3 Lo = vec3(0.0);
	F0 = vec3(0.04); 
    F0 = mix(F0, trueAlbedo, metallic);

    Lo = max(Radiance(trueNormal), 0);

    vec2 worldTexPos = ((fragPos.xz - mapMins)/mapDimensions).xy;
    worldTexPos.y = 1 - worldTexPos.y;

    vec3 roomAmbience = 8.0 * pow(texture(roomLight, worldTexPos).rgb, vec3(2.2));
    vec3 result = Lo + roomAmbience * trueAlbedo;
    result *= ambientIntensity;
    float SSAOvalue = texture(screenSSAO, texCoords).r;
    result *= SSAOvalue;
    result += emission.rgb;
    FragColour = vec4(result, 1.0);
}

vec3 Radiance(
    vec3 normal
)
{
    float attenuation = dot(normal, -lightDirection);
    vec3 radiance  = lightColour * attenuation;        
           
    return trueAlbedo * radiance; 
}
