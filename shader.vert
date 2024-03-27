#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;




//struct DirectionalLight {
//    vec3 direction;
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//};
//
//struct PointLight {
//    vec3 position;
//    float constant;
//    float linear;
//    float quadratic;
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//};

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

//#define MAX_POINT_LIGHTS 4
//uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Spotlight spotlight;




out vec3 FragPos;
//out vec3 Normal;
out vec2 TexCoords;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

out vec3 TangentSpotlightPos;
//out vec3 TangentPointLightsPos[MAX_POINT_LIGHTS];


uniform mat4 model;
uniform mat4 vp; // View projection
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
//    Normal = mat3(transpose(inverse(model))) * aNormal;  

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);
    
    // The transpose of an orthogonal matrix (each axis is a perpendicular unit vector) equals its inverse
    mat3 TBN = transpose(mat3(T, B, N));


//    vs_out.TangentLightPos = TBN * lightPos;

    TangentSpotlightPos = TBN * spotlight.position;

//    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
//        TangentPointLightsPos[i] = TBN * pointLights[i].position;
//    }
//    
    
    
    TangentViewPos  = TBN * viewPos;


    TangentFragPos  = TBN * FragPos;
    TexCoords = aTexCoords;

    gl_Position = vp * vec4(FragPos, 1.0);
}