#pragma once

#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "Model.h"

#include <string>

// TODO: Consider breaking this code up into seperate manager classes with a parent class manager for shared functionality

//TODO: Maybe get shader without needing both the fragment and vertex shader, it could just find them both if they have the same name and just differing extension or custom file format that stores the name of the others
class ResourceManager {
public:
	// TODO: Make resources be gathered from a GUID
	static Shader* LoadShader(std::string vertexPath, std::string fragmentPath, int flags = 0);
	static Shader* LoadShader(std::string sharedName, int flags = 0);
	static Shader* LoadShaderDefaultVert(std::string fragmentName, int flags = 0);
	static Model* LoadModel(std::string path);
	static Model* LoadModel();
	static Mesh* LoadMesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	static Mesh* LoadMesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
	static Mesh* LoadMesh(Mesh::presets preset);
	static Mesh* LoadMesh();
	static Texture* LoadTexture(std::string path, Texture::Type type, int wrappingMode = GL_REPEAT, bool flipOnLoad = true);
	// TODO: See if this function can be cleaned up
	static Texture* LoadTexture(unsigned int width = 1024, unsigned int height = 1024, GLenum format = GL_SRGB, unsigned char* data = nullptr, GLint wrappingMode = GL_REPEAT, GLenum dataType = GL_UNSIGNED_BYTE, bool mipMaps = false, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);
	// TODO: Should I be using hint file?
	static Material* GetMaterial(unsigned long long GUID);
	static Shader* GetShader(unsigned long long GUID);
	static Texture* GetTexture(unsigned long long GUID);
	static Model* GetModel(unsigned long long GUID);
	static Mesh* GetMesh(unsigned long long GUID);
	static Material* LoadMaterial(std::string name, Shader* shader = nullptr);

	static unsigned long long guidCounter;
	static unsigned long long GetNewGuid();

	static void BindFlaggedVariables();
	static void BindFlaggedVariables(Shader* shader);

	static void UnloadAll();
	~ResourceManager();
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;

	static void GUI();
private:
	struct hashFNV1A {
		static const unsigned long long offset;
		static const unsigned long long prime;
		unsigned long long operator()(unsigned long long key) const;
		unsigned long long operator()(std::string key) const;
	};

	// TODO: Maybe make a template function, all the 'Get' functions are very similar and could prob be written better

	static std::unordered_map<unsigned long long, Texture, hashFNV1A> textures; // This stores/owns the textures
	static std::unordered_map<unsigned long long, Shader, hashFNV1A> shaders;
	static std::unordered_map<unsigned long long, Material, hashFNV1A> materials;
	static std::unordered_map<unsigned long long, Model, hashFNV1A> models;
	static std::unordered_map<unsigned long long, Mesh, hashFNV1A> meshes;
};