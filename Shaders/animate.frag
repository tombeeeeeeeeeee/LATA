#version 460 core
struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    float shininess;
}; 
out vec4 FragColor;

in vec2 TexCoord;
in vec4 daBoneWeights;
in vec3 fragPos;


uniform Material material;

void main()
{
    FragColor = vec4(fragPos, 1.0);
//    if (isnan(fragPos.x)){
//        FragColor = vec4(1, 1, 0, 1);
//    }
//    else{
//    	FragColor = vec4(daBoneWeights.x * 0.1, daBoneWeights.y, daBoneWeights.z, 1);
//    }
//	FragColor = texture(material.diffuse1, TexCoord);
    //FragColor = vec4(TexCoord, 1, 1);
}