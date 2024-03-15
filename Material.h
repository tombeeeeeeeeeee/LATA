#pragma once

#include "Texture.h"
#include "Shader.h"

class Material
{
public:
	Material();
	Material(Texture* _diffuse, Texture* _specular, Shader* _shader);

	Texture* diffuse;
	Texture* specular;
	Shader* shader;
	
	void Use();

};

