#pragma once
#include "Texture.h"

#include "Graphics.h"

#include <string>

class TextureManager {
public:
	static Texture* GetTexture(std::string path, Texture::Type type, int wrappingMode = GL_REPEAT);

	static void Unload();
	~TextureManager();
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
private:
	struct hashThing {
		unsigned long long operator()(std::string key) const;
	};
	static GLuint LoadTexture(std::string path, int wrappingMode);
	static std::unordered_map<std::string, Texture, hashThing> loadedTextures;
	//TODO: Keep a track of amount of loaded textures and unload them as they are unneeded
};