#version 460 core
out vec4 FragColor;

struct Material {
    sampler2D normal1;
    sampler2D albedo1;
    sampler2D metallic1;
    sampler2D roughness1;
    sampler2D ao1;
}; 

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Spotlight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};

#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight directionalLight;
uniform Spotlight spotlight;

const float alphaDiscard = 0.5;

in vec3 FragPos;
in vec2 TexCoords;

in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 TangentSpotlightPos;
in vec3 TangentPointLightsPos[MAX_POINT_LIGHTS];

in mat3 inverseTBN;
  
uniform Material material;

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

#define PI 3.1415926535

void main()
{
    //FragColor = vec4(0, 1, 0.5, 1);
    //return;
    albedo = texture(material.albedo1, TexCoords).rgb;
    metallic = texture(material.metallic1, TexCoords).r;
    roughness = texture(material.roughness1, TexCoords).r;
    ao = texture(material.ao1, TexCoords).r;

    vec3 normal = texture(material.normal1, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    viewDir = normalize(TangentViewPos - TangentFragPos);

    F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // Lighting
    //TODO: should there be ifs here to check if the lights are actually 'active', I don't think it'll actually help performance wise?
    vec3 result;

    // Directional light
    result = max(CalcDirectionalLight(directionalLight, inverseTBN * normal), 0);

    // Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += clamp(CalcPointLight(pointLights[i], i, normal), 0, 1);
    }
    
    // Spot light
    result += max(CalcSpotlight(spotlight, normal), 0);

    // Alpha discarding 
//    if(texture(material.albedo1, TexCoords).a < alphaDiscard) {
//        discard;
//    }
    FragColor = vec4(result, 1.0);
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

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 Radiance(vec3 lightDir, float distanceToLight, vec3 normal, float constant, float linear, float quadratic, vec3 diffuse) {
    
    // calculate per-light radiance
    vec3 H = normalize(viewDir + lightDir);
    float attenuation = CalcAttenuation(constant, linear, quadratic, distanceToLight);
    // float attenuation = 1.0 / (distance * distance); // TODO: Switch with function?
    vec3 radiance     = diffuse * attenuation;        
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);      
    vec3 F    = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);       
        
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

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = light.ambient * albedo;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    // TODO: Fix this for pbr workflow
//    // Specular
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), metallic);
//    vec3 specular = light.specular * spec * albedo;

    // Combine results
    // TODO: fix
//    return ambient + diffuse + specular;
    return ambient + diffuse;
}

vec3 CalcPointLight(PointLight light, int i, vec3 normal)
{
    vec3 radiance = Radiance(normalize(TangentPointLightsPos[i] - TangentFragPos), length(TangentPointLightsPos[i] - TangentFragPos), normal, light.constant, light.linear, light.quadratic, light.diffuse);

    vec3 ambient = light.ambient * albedo;

    return radiance + ambient;

//    // Specular
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = light.specular * spec * vec3(specularColour);

//    // Attenuation
//    float distanceToLight = length(TangentPointLightsPos[i] - TangentFragPos);
//    float attenuation = CalcAttenuation(light.constant, light.linear, light.quadratic, distanceToLight);
//    ambient  *= attenuation;
//    diffuse  *= attenuation;
    //specular *= attenuation;

    // Combine results
    //return ambient + diffuse + specular;
    
}

vec3 CalcSpotlight(Spotlight light, vec3 normal) {
//    
//    vec3 lightDir = normalize(TangentSpotlightPos - TangentFragPos);
//
    // Spotlight stuff
    float theta = dot(normalize(light.position - FragPos), normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 radiance = Radiance(normalize(TangentSpotlightPos - TangentFragPos), length(TangentSpotlightPos - TangentFragPos), normal, light.constant, light.linear, light.quadratic, light.diffuse);

    vec3 ambient = light.ambient * albedo;

    return (radiance + ambient) * intensity;     
//    
//    ambient  *= attenuation * intensity;
//    diffuse  *= attenuation * intensity;
//    specular *= attenuation * intensity;
//
//    // Combine results
//    return ambient + diffuse + specular;
}

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight) {
    return 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
}
