#version 460 core

in vec3 lightColour;

out vec4 finalColour;

in vec3 fragPos;
in vec3 fragmentNormal;

uniform float lerpAmount;
uniform float linear;
uniform float quad;
uniform vec3 lightQ;
uniform vec3 lightP;

uniform vec3 camPos;
uniform mat4 invP;
uniform mat4 invV;
uniform vec2 invViewPort;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2D pbr;
uniform sampler2D SSAO;

vec3 trueNormal;
vec3 trueAlbedo;
float metallic;
float roughness;

vec3 viewDirection;
vec3 F0;
vec3 worldPos;
vec3 posToLight;
vec3 lightPos;
vec2 texCoords;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 Radiance(
    vec3 lightDir, float distanceToLight, 
    vec3 normal, float constant, 
    float linear, float quadratic, vec3 diffuse
);
vec3 CalcDirectionalLight(vec3 lightDirection, vec3 normal);
vec3 specularIBL(vec3 trueNormal);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight);


#define PI 3.1415926535

const float MAX_REFLECTION_LOD = 4.0;

const int sssSteps = 32;
const float sssMaxRayDistance = 2000;
const float sssThickness = 142;
const float RPC_16  = 0.0625f;


void main()
{
    texCoords = gl_FragCoord.xy;
    texCoords *= invViewPort;
	vec4 albedo4 = texture(albedo, texCoords);
	vec4 normal4 = texture(normal, texCoords);
    vec4 pbr4 = texture(pbr, texCoords);

	trueNormal = normal4.xyz;
	trueAlbedo = albedo4.xyz;
	metallic = pbr4.r;
	roughness =  pbr4.g;

	float depthValue = texture(depth, texCoords).r;

    vec4 NDC = vec4(texCoords * 2.0 - 1.0, depthValue * 2.0 - 1.0, 1.0);
    vec4 clipPos = invP * NDC;
    vec3 screenPosition = clipPos.xyz / clipPos.w;
    worldPos = (invV * vec4(screenPosition, 1.0)).xyz;
    float lightBar = dot(lightQ - lightP, lightQ - lightP);
    float t = dot(worldPos - lightP, lightQ - lightP);
    t /= lightBar;

    if(t <= 0)
    {
        lightPos = lightP;
    }
    else if(t >= 1)
    {
        lightPos = lightQ;
    }
    else
    {
        lightPos = lightP + t*(lightQ - lightP);
    }

	posToLight = lightPos - worldPos;

	viewDirection = -normalize(worldPos - (camPos));
	F0 = vec3(0.04); 
	F0 = mix(F0, trueAlbedo, metallic);

	vec3 Lo = clamp(Radiance(
	normalize(posToLight),
	length(posToLight) / 100.0,
	trueNormal, 1, linear, quad, lightColour
	), 0, 1);

	finalColour = vec4(Lo, 1.0);
	return;
}

vec3 Radiance(
    vec3 lightDir, float distanceToLight, 
    vec3 normal, float constant, 
    float linear, float quadratic, vec3 diffuse
)
{
    // calculate per-light radiance
    vec3 H = normalize(viewDirection + lightDir);
    float attenuation = CalcAttenuation(constant, linear, quadratic, distanceToLight);
    vec3 radiance     = diffuse * attenuation;        
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, viewDirection, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDirection), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);                
    return (kD * trueAlbedo / PI + specular) * radiance * NdotL; 
}

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight) {
    return 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}