#pragma once
#include "Texture.h"
#include "Shader.h"

#include "Graphics.h"

#include <string>

//TODO: Maybe get shader without needing both the fragment and vertex shader, it could just find them both if they have the same name and just differing extension
class ResourceManager {
public:
	static Texture* GetTexture(std::string path, Texture::Type type, int wrappingMode = GL_REPEAT);
	static Shader* GetShader(std::string vertexPath, std::string fragmentPath);

	static void Unload();
	~ResourceManager();
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
private:
	struct hashFNV1A {
		unsigned long long operator()(std::string key) const;
	};
	static std::unordered_map<std::string, Texture, hashFNV1A> textures;
	static std::unordered_map<std::string, Shader, hashFNV1A> shaders;
	//TODO: Keep a track of amount of loaded textures and unload them as they are unneeded
};