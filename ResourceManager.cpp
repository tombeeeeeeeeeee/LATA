#include "ResourceManager.h"

#include "stb_image.h"

#include <iostream>

std::unordered_map<std::string, Texture, ResourceManager::hashFNV1A> ResourceManager::textures;
std::unordered_map<std::string, Shader, ResourceManager::hashFNV1A> ResourceManager::shaders;

/// <summary>
/// Gets a texture from a path
/// </summary>
/// <param name="path">The path to the texture</param>
/// <param name="wrappingMode">The texture wrapping mode to use, this value will be ignored if the texture is already loaded</param>
/// <returns>Pointer to the texture at the given path</returns>
Texture* ResourceManager::GetTexture(std::string path, Texture::Type type, int wrappingMode)
{
	auto texture = textures.find(path);

	if (texture == textures.end()) {
		Texture newTexture;
		newTexture.ID = Texture::Load(path.c_str(), wrappingMode);
		newTexture.type = type;

		texture = textures.emplace(path, newTexture).first;
	}

	return &texture->second;
}

Shader* ResourceManager::GetShader(std::string vertexPath, std::string fragmentPath)
{
	std::string path = vertexPath + fragmentPath;
	auto shader = shaders.find(path);

	if (shader == shaders.end()) {
		Shader newShader;
		newShader.ID = Shader::Load(vertexPath, fragmentPath);

		shader = shaders.emplace(path, newShader).first;
	}

	return &shader->second;
	return nullptr;
}

unsigned long long ResourceManager::hashFNV1A::operator()(std::string key) const
{
	unsigned long long hash = 14695981039346656037;
	for (auto i = 0; i < key.size(); i++)
	{
		hash ^= key[i];
		hash *= 1099511628211;
	}
	return hash;
}

ResourceManager::~ResourceManager()
{
	Unload();
}

void ResourceManager::Unload()
{
	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		glDeleteTextures(1, &i->second.ID);
	}
	textures.clear();

	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		i->second.DeleteProgram();
	}
}