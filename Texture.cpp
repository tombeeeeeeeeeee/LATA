#include "Texture.h"

#include "Graphics.h"

#include "stb_image.h"

#include <iostream>

const std::unordered_map<Texture::Type, std::string> Texture::TypeNames = 
{
	{ Type::diffuse, "diffuse" },
	{ Type::specular, "specular" },
	{ Type::normal, "normal" },
	{ Type::height, "height" },
	{ Type::emission, "emission" },
};

Texture::Texture() : 
	ID(-1)
{
}
