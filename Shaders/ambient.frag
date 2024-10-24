#version 460 core

layout (location = 0) out vec4 FragColour;
in vec2 texCoords;

uniform sampler2D screenDepth;
uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenEmission;
uniform sampler2D screenSSAO;

uniform samplerCube skybox;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;	
uniform sampler2D brdfLUT;

uniform mat4 invP;
uniform mat4 invVP;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 camPos;

vec3 viewDirection;
vec3 F0;
vec3 trueAlbedo;
float roughness;
float metallic;
float ao;
vec3 screenPos;
vec3 fragPos;

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
vec3 specularIBL(vec3 trueNormal, vec3 viewDirection);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight);
#define PI 3.1415926535
const float MAX_REFLECTION_LOD = 4.0;

void main()
{
	float depthValue = texture(screenDepth, texCoords).r;
    vec4 NDC = vec4(gl_FragCoord.xy, depthValue, 1.0);
    screenPos = (invP * NDC).rgb;
    fragPos = (invVP * NDC).rgb;

    viewDirection = normalize(fragPos - camPos);

    if(depthValue >= 1.0)
    {
        FragColour = texture(skybox, viewDirection);
    }
    else
    {
        vec4 emission = texture(screenEmission, texCoords);
        vec4 albedo = texture(screenAlbedo, texCoords);
        vec4 normal = texture(screenNormal, texCoords);
        vec3 trueNormal = normal.rgb;
        trueAlbedo = albedo.rgb;
        roughness = normal.a;
        metallic = albedo.a;
        ao = emission.a;

        vec3 Lo = vec3(0.0);
	    F0 = vec3(0.04); 
        F0 = mix(F0, trueAlbedo, metallic);

        Lo = max(CalcDirectionalLight(lightDirection, trueNormal), 0);
        vec3 IBL = specularIBL(trueNormal, viewDirection);
        vec3 result = Lo + IBL;
        FragColour = vec4(result, 1.0);
    }
}

vec3 CalcDirectionalLight(vec3 lightDirection, vec3 normal)
{
	vec3 radiance = Radiance(-lightDirection, 1, normal, 1, 0, 0, lightColour);

    return radiance;
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
    vec3 radiance  = diffuse * attenuation;        
        
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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 specularIBL(vec3 trueNormal, vec3 viewDirection)
{
    
    vec3 reflected = reflect(-viewDirection, trueNormal); 
	vec3 kS = fresnelSchlickRoughness(max(dot(trueNormal, viewDirection), 0.0), F0, roughness);
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, trueNormal).rgb;
    float SSAOvalue = texture(screenSSAO, texCoords).r;

    vec3 diffuse = irradiance * trueAlbedo;
	//vec3 additionalAmbient = (kD * ambientLightColour) * ao;
    vec3 prefilteredColor = textureLod(prefilterMap, reflected,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(trueNormal, viewDirection), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * 1.0;
    ambient *= SSAOvalue * ao;

    return ambient;
}

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight) {
    return 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
}