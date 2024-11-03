#version 460 core

in vec2 texCoords;
in vec3 lightColour;

layout (location = 0) out vec4 finalColour;

in vec2 screenPos;
in vec3 fragmentNormal;

uniform float linear;
uniform float quad;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform vec3 cameraDelta;
uniform mat4 invVP;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;

vec3 trueNormal;
vec3 trueAlbedo;
float metallic;
float roughness;

vec3 viewDirection;
vec3 F0;

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


void main()
{
	vec4 albedo4 = texture(albedo, screenPos);
	vec4 normal4 = texture(normal, screenPos);

	trueNormal = normal4.xyz;
	trueAlbedo = albedo4.xyz;
	metallic = albedo4.w;
	roughness = normal4.w;

	float depthValue = texture(screenDepth, screenPos).r;
    vec4 NDC = vec4(texCoords * 2.0 - 1.0, depthValue * 2.0 - 1.0, 1.0);
    vec4 clipPos = invP * NDC;
    screenPos = clipPos.xyz / clipPos.w;
    fragPos = (invV * vec4(screenPos, 1.0)).xyz;

	vec3 posToLight = lightPos - fragPos;
	if(dot(posToLight, trueNormal) < 0.0)
	{
		finalColour = vec4(0.0);
		return;
	}
	else
	{
		vec3 viewDirection = normalize(fragmentPos - (camPos - cameraDelta));
		F0 = vec3(0.04); 
		F0 = mix(F0, treuAlbedo, metallic);

		vec3 Lo = clamp(Radiance(
		normalize(posToLight),
		length(posToLight),
		trueNormal, 1, linear, quad, lightColour
		), 0, 1);
		finalColour = vec4(Lo, 1.0);
		return;
	}
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
    return (kD * albedo / PI + specular) * radiance * NdotL; 
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