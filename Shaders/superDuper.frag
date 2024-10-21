#version 460 core

struct Material {
    sampler2D albedo;
    sampler2D normal;
    sampler2D PBR;
    sampler2D emission;
}; 

struct DirectionalLight {
    vec3 direction;
    vec3 colour;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 colour;
};

struct Spotlight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 colour;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};

const float alphaDiscard = 0.5;

uniform Material material;

in vec2 texCoords;
in vec3 fragmentPos;
in vec3 fragmentColour;

layout (location = 0) out vec4 screenColour;
layout (location = 1) out vec4 bloomColour;

// Lighting
#define PI 3.1415926535
const int MAX_POINT_LIGHTS = 4;


uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight directionalLight;
uniform Spotlight spotlight;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 Radiance(vec3 lightDir, float distanceToLight, vec3 normal, float constant, float linear, float quadratic, vec3 diffuse);
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal);
vec3 CalcPointLight(PointLight light, int index, vec3 normal);
vec3 CalcSpotlight(Spotlight light, vec3 normal);
float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight);

vec3 viewDir;
vec3 F0;
vec3 albedo;
float metallic;
float roughness;
float ao;

// Shadows
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform vec3 lightPos;

//SSAO
uniform sampler2D ssao;
in vec4 ndc;
vec2 screenPosition;

in vec4 directionalLightSpaceFragPos;

float ShadowCalculation(vec4 fragPosLightSpace);

// Normals
in vec3 spotlightPos;
in vec3 pointLightsPos[MAX_POINT_LIGHTS];
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;
mat3 TBN;


// IBL
uniform samplerCube irradianceMap;	
uniform samplerCube prefilterMap;	
uniform sampler2D brdfLUT;
vec3 specularIBL(vec3 trueNormal, vec3 viewDirection);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

//Brush Strokes Effect
//uniform sampler2D brushStrokes;
//uniform vec4 brushAtlasLocation;

const float MAX_REFLECTION_LOD = 4.0;

void main()
{
    vec3 PBR = texture(material.PBR, texCoords).rgb;
    albedo = texture(material.albedo, texCoords).rgb * fragmentColour;

    //TODO: Add atlasing
    //albedo *= texture(brushStrokes, texCoords).rgb;
    metallic = PBR.r;
    roughness = PBR.g;
    ao = PBR.b;
	
    screenPosition = ndc.xy/ndc.w;
    screenPosition = screenPosition * 0.5 + 0.5;

    TBN = mat3(normalize(fragmentTangent), normalize(fragmentBitangent), normalize(fragmentNormal));
    vec3 tangentNormal = texture(material.normal, texCoords).rgb;

    //Catch for when NormalMap isnt set
    if(tangentNormal.r + tangentNormal.g + tangentNormal.b == 0.0)
        tangentNormal = vec3(0.5,0.5,1.0);

    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    vec3 trueNormal = TBN * tangentNormal;
    viewDir = normalize(viewPos - fragmentPos);

    //Light Output
    vec3 Lo = vec3(0.0);

	F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    float directionalLightShadow = ( 1 - ShadowCalculation(directionalLightSpaceFragPos));
    Lo = max(CalcDirectionalLight(directionalLight, trueNormal), 0) * directionalLightShadow;

    // Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        Lo += clamp(CalcPointLight(pointLights[i], i, trueNormal), 0, 1);
    }
    
    // Spot light
    Lo += max(CalcSpotlight(spotlight, trueNormal), 0);

    vec3 IBL = specularIBL(trueNormal, viewDir);

    vec3 result = Lo + IBL;

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

	if(brightness > 1)
	{ 
		bloomColour = vec4(result, min(brightness - 1.0, 1.0));
	}
	else 
    {
        bloomColour = vec4(0.0, 0.0, 0.0, 1.0);
    }

    vec4 emissionColour = texture(material.emission, texCoords);

    bloomColour = vec4(emissionColour.rgb + bloomColour.rgb, 1.0);
    bloomColour *= emissionColour.a + 1.0;
    screenColour = vec4(result, 1.0);
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

vec3 Radiance(
    vec3 lightDir, float distanceToLight, 
    vec3 normal, float constant, 
    float linear, float quadratic, vec3 diffuse
)
{
    // calculate per-light radiance
    vec3 H = normalize(viewDir + lightDir);
    float attenuation = CalcAttenuation(constant, linear, quadratic, distanceToLight);
    // float attenuation = 1.0 / (distance * distance); // TODO: Switch with function?
    vec3 radiance     = diffuse * attenuation;        
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);                
    return (kD * albedo / PI + specular) * radiance * NdotL; 
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 n = normalize(fragmentNormal);
    vec3 lightDir = normalize(lightPos - fragmentPos);
    float bias = max(0.05 * (1.0 - dot(n, lightDir)), 0.005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal)
{
	vec3 radiance = Radiance(-light.direction, 1, normal, 1, 0, 0, light.colour);

    return radiance;
}

vec3 CalcPointLight(PointLight light, int i, vec3 normal)
{
    vec3 radiance = Radiance(
    normalize(pointLightsPos[i] - fragmentPos), 
    length(pointLightsPos[i] - fragmentPos),
    normal, 
    light.constant, light.linear, light.quadratic, light.colour);

    return radiance;
}

vec3 CalcSpotlight(Spotlight light, vec3 normal) 
{
    // Spotlight stuff
    float theta = dot(normalize(light.position - fragmentPos), normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 radiance = Radiance(
    normalize(spotlightPos - fragmentPos), 
    length(spotlightPos - fragmentPos),
    normal, 
    light.constant, light.linear, light.quadratic, light.colour);

    return radiance * intensity;
}

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight) {
    return 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
}

vec3 specularIBL(vec3 trueNormal, vec3 viewDirection)
{
    
    vec3 reflected = reflect(-viewDirection, trueNormal); 
	vec3 kS = fresnelSchlickRoughness(max(dot(trueNormal, viewDirection), 0.0), F0, roughness);
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
    vec3 irradiance = fragmentColour * texture(irradianceMap, trueNormal).rgb;
    float SSAO = texture(ssao, screenPosition).r;

    vec3 diffuse = irradiance * albedo;
	//vec3 additionalAmbient = (kD * ambientLightColour) * ao;
    vec3 prefilteredColor = textureLod(prefilterMap, reflected,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(trueNormal, viewDirection), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * 1.0;
    ambient *= SSAO * SSAO;

    return ambient;
}