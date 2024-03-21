#include "TextureManager.h"

#include "stb_image.h"

#include <iostream>

std::unordered_map<std::string, Texture, TextureManager::hashThing> TextureManager::loadedTextures;

/// <summary>
/// Gets a texture from a path
/// </summary>
/// <param name="path">The path to the texture</param>
/// <param name="wrappingMode">The texture wrapping mode to use, this value will be ignored if the texture is already loaded</param>
/// <returns>Pointer to the texture at the given path</returns>
Texture* TextureManager::GetTexture(std::string path, Texture::Type type, int wrappingMode)
{
	auto texture = loadedTextures.find(path);

	if (texture == loadedTextures.end()) {
		Texture newTexture;
		newTexture.ID = LoadTexture(path.c_str(), wrappingMode);
		newTexture.type = type;

		texture = loadedTextures.emplace(path, newTexture).first;
	}

	return &texture->second;
}

unsigned long long TextureManager::hashThing::operator()(std::string key) const
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

GLuint TextureManager::LoadTexture(std::string path, int wrappingMode)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		switch (nrComponents)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4: 
			format = GL_RGBA;
			break;
		case 2:
		default:
			std::cout << "Texture failed to load, could not be read correctly, path: " << path << "\n";
			stbi_image_free(data);
			return textureID;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load, path: " << path << "\n";
	}
	stbi_image_free(data);

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