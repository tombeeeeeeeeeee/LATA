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

GLuint Texture::Load(std::string path, int wrappingMode)
{
	GLuint ID;
	glGenTextures(1, &ID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, STBI_default);
	if (data)
	{
		GLenum format;
		switch (nrComponents)
		{
		case STBI_grey:
			format = GL_RED;
			break;
		case STBI_grey_alpha:
			format = GL_RG;
			break;
		case STBI_rgb:
			format = GL_RGB;
			break;
		case STBI_rgb_alpha:
			format = GL_RGBA;
			break;
		default:
			std::cout << "Texture failed to load, could not be read correctly, path: " << path << "\n";
			stbi_image_free(data);
			return ID;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
		//TODO:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load, path: " << path << "\n";
	}
	stbi_image_free(data);

	return ID;
}
