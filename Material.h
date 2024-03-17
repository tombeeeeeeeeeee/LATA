#pragma once

#include "Texture.h"
#include "Shader.h"

class Material
{
public:
	Material();
	Material(Texture* _diffuse, Texture* _specular, Texture* _emission, float _shininess = 64.0f);

	Texture* diffuse;
	Texture* specular;
	Texture* emission;
	float shininess;
	
	void Use();

};

