#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    float shininess;
}; 

struct DirLight {
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

struct SpotLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
};

#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];


uniform DirLight dirLight;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(Spot)

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Directional light
    //TODO: should there be ifs here to check if the lights are actually 'active'
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    
    // Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos)


    // emission
    vec3 emission1 = vec3(texture(material.emission1, TexCoords));


    result += emission1;
    FragColor = vec4(result, 1.0);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
