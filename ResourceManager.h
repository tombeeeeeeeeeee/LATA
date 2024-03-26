#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Material.h"

#include "Graphics.h"

#include <string>

//TODO: Maybe get shader without needing both the fragment and vertex shader, it could just find them both if they have the same name and just differing extension or custom file format that stores the name of the others
class ResourceManager {
public:
	friend class GUI;
	//TODO: Make the same texture be able to be loaded multiple times as different types?
	/// <summary>
	/// Gets a texture from a path
	/// </summary>
	/// <param name="path">The path to the texture</param>
	/// <param name="type">The texture type, this value will be ignored if the texture is already loaded</param>
	/// <param name="wrappingMode">The texture wrapping mode to use, this value will be ignored if the texture is already loaded</param>
	/// <returns>Pointer to the texture at the given path</returns>	
	static Texture* GetTexture(std::string path, Texture::Type type, int wrappingMode = GL_REPEAT, bool flipOnLoad = true);
	static Shader* GetShader(std::string vertexPath, std::string fragmentPath);
	static Material* GetMaterial(std::vector<Texture*> textures);

	static void Unload();
	~ResourceManager();
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
private:
	struct hashFNV1A {
		static const unsigned long long offset;
		static const unsigned long long prime;
		unsigned long long operator()(std::string key) const;
		unsigned long long operator()(std::vector<Texture*> key) const;

	};
	static std::unordered_map<std::string, Texture, hashFNV1A> textures;
	static std::unordered_map<std::string, Shader, hashFNV1A> shaders;
	static std::unordered_map<std::vector<Texture*>, Material, hashFNV1A> materials;
};