#pragma once
#include <string>
#include "Texture.h"

#include "glad.h"

class TextureManager {
public:
	static Texture* GetTexture(std::string path, int wrappingMode = GL_REPEAT);

	static void Unload();
	~TextureManager();
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
private:
	struct hashThing {
		unsigned long long operator()(std::string key) const;
	};
	static unsigned int LoadTexture(std::string path, int wrappingMode);
	static std::unordered_map<std::string, Texture, hashThing> loadedTextures;
	//TODO: Keep a track of amount of loaded textures and unload them as they are unneeded
};