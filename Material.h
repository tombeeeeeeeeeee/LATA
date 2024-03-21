#pragma once

#include "Texture.h"
#include "Shader.h"

#include <vector>

//TODO: being unused really, decide what to do with
struct Material
{
public:
	Material();
	Material(unsigned int textureCount, Texture* _textures);
	Material(std::vector<Texture*> _textures);
	

	std::vector<Texture*> textures;
	Shader* shader;
	void Use();

};

