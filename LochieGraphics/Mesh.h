#pragma once
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "BoneInfo.h"

#include "Maths.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"



// TODO: Meshes cannot be shared across models atm, as they are owned by the models
class Mesh
{
private:
	unsigned int triCount;
	GLuint VAO, VBO, IBO;

public:
	enum class presets {
		cube,
		quad,
		doubleQuad,
		cubeOppositeWind
	};
	static int aiLoadFlag;

	void Draw();

	// TODO: Constructors for preset shapes 
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
	Mesh(presets preset);
	Mesh();
	~Mesh();

	Mesh(const Mesh& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;

	// Move constructor
	Mesh(Mesh&& other) noexcept;
	Mesh& operator = (Mesh&& other) = delete;

	// Pre set
	//TODO: add more and customisable, specifically sphere
	// UV Sphere
	// Quad Sphere
	// Goldberg Polyhedra
	void InitialiseQuad(float size = 0.5f, float offset = 0.0f);
	void InitialiseDoubleSidedQuad();
	void InitialiseCube();
	void InitialiseCubeInsideOut();

	void InitialiseFromFile(std::string filename);
	void InitialiseIndexFromFile(std::string filename, int i);
	void InitialiseFromAiMesh(std::string path, const aiScene* scene, std::unordered_map<std::string, BoneInfo>* boneInfo, aiMesh* mesh, bool flipTexturesOnLoad = true);
private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void GenAndBind();
	void Unbind();
	//TODO: Both the aiTextureType and the Texture::Type shouldn't be passed, make a dictionary or something and only pass one in
	//static std::vector<Texture*> LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad = true);
	//static void AddMaterialTextures(std::vector<Texture*>* textures, std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad = true);
};