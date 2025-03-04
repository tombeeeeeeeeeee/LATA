#pragma once

#include "Texture.h"
#include "Shader.h"
#include "Material.h"

#include "hashFNV1A.h"

#include <random>

// TODO: Consider breaking this code up into seperate manager classes with a parent class manager for shared functionality

class ResourceManager {
public:
	static Shader* LoadShader(std::string vertexPath, std::string fragmentPath, int flags = 0);
	static Shader* LoadShader(std::string sharedName, int flags = 0);
	// TODO: Make resources be gathered from a GUID
	static Shader* LoadShaderDefaultVert(std::string fragmentName, int flags = 0);
	static Shader* LoadShader(toml::v3::table* toml);
	static Shader* LoadShader(toml::v3::table toml);
	static Shader* LoadShaderAsset(std::string path);
	static Texture* LoadTexture(std::string path, Texture::Type type, int wrappingMode = GL_REPEAT, bool flipOnLoad = true);
	// TODO: See if this function can be cleaned up
	static Texture* CreateTexture(unsigned int width = 1024, unsigned int height = 1024, GLenum format = GL_SRGB, unsigned char* data = nullptr, GLint wrappingMode = GL_REPEAT, GLenum dataType = GL_UNSIGNED_BYTE, bool mipMaps = false, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);
	static Texture* LoadTextureAsset(std::string path);
	// TODO: Should I be using hint file?
	// TODO: These are the some of the only few getters with a cap 'g'
	static Material* GetMaterial(unsigned long long GUID);
	static Shader* GetShader(unsigned long long GUID);
	static Texture* GetTexture(unsigned long long GUID);
	// TODO: Remove this nullptr default for the shader reference, a material can't really exist without a shader
	// TODO: Should really be called create instead of load
	static Material* LoadMaterial(std::string name, Shader* shader = nullptr);
	static Material* LoadDefaultMaterial();
	static Material* LoadMaterialAsset(std::string path);

	static std::unordered_map<unsigned long long, Material, hashFNV1A>& getMaterials();

	// Selector doesn't need a tag for label, just label is fine
	static bool TextureSelector(std::string label, Texture** texture, bool showNull = true);
	// Selector doesn't need a tag for label, just label is fine
	static bool ShaderSelector(std::string label, Shader** shader, bool showNull = false);
	// Selector doesn't need a tag for label, just label is fine
	static bool MaterialSelector(std::string label, Material** material, Shader* newMaterialShader = nullptr, bool showNull = true);


	static unsigned long long guidCounter;
	static std::random_device guidRandomiser;
	static unsigned long long GetNewGuid();

	static void BindFlaggedVariables();
	static void BindFlaggedVariables(Shader* shader);

	static void UnloadShaders();
	static void UnloadAll();

	static void RefreshAllMaterials();

	~ResourceManager();
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
	// TODO: Delete constructor as well?

	static void GUI();


	// TODO: Maybe make a template function, all the 'Get' functions are very similar and could prob be written better

	static std::unordered_map<unsigned long long, Texture, hashFNV1A> textures; // This stores/owns the textures
	static std::unordered_map<unsigned long long, Shader, hashFNV1A> shaders;
	static std::unordered_map<unsigned long long, Material, hashFNV1A> materials;

	static Texture* defaultTexture;
	static Texture* defaultNormal;
	static Shader* defaultShader;
	static Material* defaultMaterial;

	static Shader* shadowMapDepth;
	static Shader* shadowDebug;
	static Shader* screen;
	static Shader* brdf;
	static Shader* prefilter;
	static Shader* downSample;
	static Shader* upSample;
	static Shader* irradiance;
	static Shader* lines;
	static Shader* ssao;
	static Shader* ssaoBlur;
	static Shader* prepass;
	static Shader* super;
};