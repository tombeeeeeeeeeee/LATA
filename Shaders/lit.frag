#version 460 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
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

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight);

vec3 viewDir;

void main()
{
    vec3 norm = normalize(Normal);
    viewDir = normalize(viewPos - FragPos);

    // Directional light
    //TODO: should there be ifs here to check if the lights are actually 'active', I don't think it'll actually help performance wise?
    vec3 result = max(CalcDirectionalLight(directionalLight, norm), 0);

    // Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += max(CalcPointLight(pointLights[i], norm), 0);
    }
    
    // Spot light
    result += max(CalcSpotlight(spotlight, norm), 0);

    // Emission
    result += vec3(texture(material.emission1, TexCoords));

    if(texture(material.diffuse1, TexCoords).a < alphaDiscard) {
        discard;
    }

    FragColor = vec4(result, 1.0);
} 

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

    // Combine results
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal)
{
    vec3 lightDir = normalize(light.position - FragPos);

    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

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
    vec3 ambient = light.ambient * texture(material.diffuse1, TexCoords).rgb;
	ambient *= intensity;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse1, TexCoords).rgb;
    diffuse *= intensity;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular1, TexCoords).rgb;
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