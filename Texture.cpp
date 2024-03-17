#include "Texture.h"

#include "glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "stb_image.h"

std::unordered_map<std::string, Texture, hashThing> TextureManager::loadedTextures;

Texture::Texture() : 
	ID(-1)
{
}



Texture* TextureManager::GetTexture(std::string path)
{
	auto texture = loadedTextures.find(path);

	if (texture == loadedTextures.end()) {

		Texture newTexture;
		newTexture.ID = LoadTexture(path.c_str());

		texture = loadedTextures.emplace(path, newTexture).first;
	}

	return &texture->second;
}

unsigned long long hashThing::operator()(std::string key) const
{
	unsigned long long hash = 14695981039346656037;
	for (auto i = 0; i < key.size(); i++)
	{
		hash ^= key[i];
		hash *= 1099511628211;
	}
	return hash;
}

TextureManager::~TextureManager()
{
	Unload();
}

unsigned int TextureManager::LoadTexture(std::string path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
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

void TextureManager::Unload()
{
	for (auto i = loadedTextures.begin(); i != loadedTextures.end(); i++)
	{
		glDeleteTextures(1, &i->second.ID);
	}

	loadedTextures.clear();
}
