#version 460 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 bloomColour;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D normal1;
    float shininess;
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
in vec3 Normal;
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal);
vec3 CalcPointLight(PointLight light, vec3 normal);
vec3 CalcSpotlight(Spotlight light, vec3 normal);

float ShadowCalculation(vec4 fragPosLightSpace);

uniform sampler2D shadowMap;

in vec4 FragPosLightSpace;

uniform vec3 lightPos;


float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight);

vec3 viewDir;
vec4 diffuseColour;
vec4 specularColour;

void main()
{

    vec3 color = texture(material.diffuse1, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
    if(dot(lighting, vec3(0.2126, 0.7152, 0.0722)) > 1)
    {
        bloomColour = vec4(lighting,1.0);
    }
    else
    {
        bloomColour = vec4(0.0,0.0,0.0,1.0);
    }
//    vec3 diffuseColour = texture(material.diffuse1, TexCoords).rgb;
//    vec3 normal = normalize(Normal);
//    vec3 lightColor = vec3(0.3);
//    // ambient
//    vec3 ambient = 0.3 * lightColor;
//    // diffuse
//    vec3 lightDir = normalize(lightPos - FragPos);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
//    // specular
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = 0.0;
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//    vec3 specular = spec * lightColor;    
//    // calculate shadow
//    float shadow = ShadowCalculation(FragPosLightSpace);                      
//    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuseColour;    
//    
//    FragColor = vec4(lighting, 1.0);
//



//    diffuseColour = texture(material.diffuse1, TexCoords);
//    specularColour = texture(material.specular1, TexCoords);
//
//    vec3 norm = normalize(Normal);
//    viewDir = normalize(viewPos - FragPos);
//
//    // Directional light
//    //TODO: should there be ifs here to check if the lights are actually 'active', I don't think it'll actually help performance wise?
//    vec3 result = max(CalcDirectionalLight(directionalLight, norm), 0);
//
//    // Point lights
//    //for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
//    //    result += max(CalcPointLight(pointLights[i], norm), 0);
//    //}
//    
//    // Spot light
//    //result += max(CalcSpotlight(spotlight, norm), 0);
//
//    //if(diffuseColour.a < alphaDiscard) {
//    //    discard;
//    //}
//
//    FragColor = vec4(result, 1.0);
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
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
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
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}  

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal)
{
    // ambient
    vec3 ambient = light.ambient * light.diffuse;
    // diffuse
    vec3 lightDir = normalize((-20.f * light.direction) - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.diffuse;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * light.diffuse;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuseColour.rgb;

    return lighting;


//    vec3 lightDir = normalize(-light.direction);
//
//    // Ambient
//    vec3 ambient = light.ambient * vec3(diffuseColour);
//
//    // Diffuse
//    float diff = max(dot(normal, lightDir), 0.0);
//    vec3 diffuse = light.diffuse * diff * vec3(diffuseColour);
//
//    // Specular
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = light.specular * spec * vec3(specularColour);
//
//    // Combine results
//    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal)
{
    vec3 lightDir = normalize(light.position - FragPos);

    // Ambient
    vec3 ambient = light.ambient * vec3(diffuseColour);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(diffuseColour);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(specularColour);

    // Attenuation
    float distanceToLight = length(light.position - FragPos);
    float attenuation = CalcAttenuation(light.constant, light.linear, light.quadratic, distanceToLight);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    // Combine results
    return ambient + diffuse + specular;
}

vec3 CalcSpotlight(Spotlight light, vec3 normal) {
    
    vec3 lightDir = normalize(light.position - FragPos);

    // Spotlight stuff
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Ambient
    vec3 ambient = light.ambient * diffuseColour.rgb;
	ambient *= intensity;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColour.rgb;
    diffuse *= intensity;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularColour.rgb;
	specular *= intensity;
    
    // Attenuation
    float distanceToLight  = length(light.position - FragPos);
    float attenuation = CalcAttenuation(light.constant, light.linear, light.quadratic, distanceToLight);    
    
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    // Combine results
    return ambient + diffuse + specular;
}

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight) {
    return 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
}