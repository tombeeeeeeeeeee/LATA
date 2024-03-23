#pragma once

#include "Texture.h"

#include <vector>

class Shader;

struct Material
{
public:
	Material();
	Material(unsigned int textureCount, Texture* _textures);
	Material(std::vector<Texture*> _textures);
	

	std::vector<Texture*> textures;
	void Use(Shader* shader);
};

