#include "Texture.h"

#include "glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "stb_image.h"

Texture::Texture(const char* path) : 
	initialised(true)
{
	ID = LoadTexture(path);
}

Texture::Texture() :
	initialised(false)
{
}

void Texture::Initialise(const char* path)
{
	ID = LoadTexture(path);
	initialised = true;
}

void Texture::Delete()
{
	if (initialised) {
		glDeleteTextures(1, &ID);
		initialised = false;
	}
}

Texture::~Texture()
{
	Delete();
}

unsigned int Texture::LoadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else {
			std::cout << "Texture failed to load, could not be read correctly, path: " << path << "\n";
			stbi_image_free(data);
			//TODO: Should this return textureID, so it can still call delete textures if that is even neccessary 
			return textureID;
		}


		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load, path: " << path << "\n";
		stbi_image_free(data);
	}

	return textureID;

}
