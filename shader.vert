#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    //sampler2D height;
    float shininess;
}; 
uniform Material material;

uniform float scale = 1.0;
uniform float offset = 0.0;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    

    //float height = texture2D(material.height, aTexCoords).r;



//    gl_Position = projection * view * vec4(FragPos + (Normal * height), 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}