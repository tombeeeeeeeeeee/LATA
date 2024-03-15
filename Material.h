#pragma once
class Material
{
public:
	Material();
	Material(unsigned int diffuse, unsigned int specular);

	unsigned int diffuseMap;
	unsigned int specularMap;
	
	void Use();

};

