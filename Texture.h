#pragma once

#include <unordered_map>
#include <string>


class Texture
{
public:
	unsigned int ID;

	Texture();

private:
	// 
	

	
};
struct hashThing {
	unsigned long long operator()(std::string key) const;
};
class TextureManager {
public:
	static Texture* GetTexture(std::string path);
	
	static void Unload();
	~TextureManager();
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
private:

	static unsigned int LoadTexture(std::string path);
	static std::unordered_map<std::string, Texture, hashThing> loadedTextures;
	//TODO: Keep a track of amount of loaded textures and unload them as they are unneeded
};
