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


class Mesh
{
public:

	enum class presets {
		cube,
		quad,
		doubleQuad,
		cubeOppositeWind
	};


	glm::vec3 max, min;
private:
	unsigned int triCount;
	GLuint VAO, VBO, IBO;


	friend class ResourceManager;
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
	Mesh(presets preset);
	Mesh();

public:
	
	static int aiLoadFlag;

	// Not a GUID
	int materialID;

	unsigned long long GUID;

	void Draw();

	~Mesh();

	//Mesh(const Mesh& other) = delete;
	//Mesh& operator=(const Mesh& other) = delete;

	////// Move constructor
	//Mesh(Mesh&& other) noexcept;
	//Mesh& operator = (Mesh&& other) = delete;

	// Pre set
	//TODO: add more and customisable, specifically sphere
	// UV Sphere
	// Quad Sphere
	// Goldberg Polyhedra
	// TODO: Change how the size counts for the quad, make it the same as how the cube does
	void InitialiseQuad(float size = 0.5f, float offset = 0.0f);
	void InitialiseDoubleSidedQuad();
	void InitialiseCube(float size = 1.0f);
	void InitialiseCubeInsideOut();

	void InitialiseFromFile(std::string filename);
	void InitialiseIndexFromFile(std::string filename, int i);
	void InitialiseFromAiMesh(std::string path, const aiScene* scene, std::unordered_map<std::string, BoneInfo>* boneInfo, aiMesh* mesh, bool flipTexturesOnLoad = true);

	unsigned int getTriCount();
	GLuint getVAO();
	GLuint getVBO();
	GLuint getIBO();

private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void GenAndBind();
	void Unbind();
	//TODO: Both the aiTextureType and the Texture::Type shouldn't be passed, make a dictionary or something and only pass one in
	//static std::vector<Texture*> LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad = true);
	//static void AddMaterialTextures(std::vector<Texture*>* textures, std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad = true);
};