#include "Texture.h"

#include "glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "stb_image.h"


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
