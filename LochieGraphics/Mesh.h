#pragma once
#include "Graphics.h"

#include "Maths.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include <vector>
#include <unordered_map>

struct BoneInfo;
struct Vertex;

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

public:
	
	static int aiLoadFlag;

	// Not a GUID, this is the material index for the belonging model
	int materialID = 0;

	unsigned long long GUID;

	void Draw();

	Mesh();
	Mesh(presets preset);
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
	void InitialiseFromAiMesh(std::string path, const aiScene* scene, std::unordered_map<std::string, BoneInfo>* boneInfo, aiMesh* mesh);

	unsigned int getTriCount() const;
	GLuint getVAO() const;
	GLuint getVBO() const;
	GLuint getIBO() const;

private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void GenAndBind();
	// Should be static?
	void Unbind();
	//TODO: Both the aiTextureType and the Texture::Type shouldn't be passed, make a dictionary or something and only pass one in
	//static std::vector<Texture*> LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad = true);
	//static void AddMaterialTextures(std::vector<Texture*>* textures, std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad = true);
};