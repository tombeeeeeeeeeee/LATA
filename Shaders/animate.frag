#version 460 core
struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    float shininess;
}; 
out vec4 FragColor;

in vec2 TexCoord;
in vec4 firstBoneID;

uniform Material material;

void main()
{
	FragColor = vec4(firstBoneID.x * 10, firstBoneID.y * 10, firstBoneID.z * 10, 1);
//	FragColor = texture(material.diffuse1, TexCoord);
    //FragColor = vec4(TexCoord, 1, 1);
}