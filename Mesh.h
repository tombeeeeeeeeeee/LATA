#pragma once
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"

#include "Maths.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"

class Mesh
{
private:
	unsigned int triCount;
	GLuint VAO, VBO, IBO;

public:
	// 
	// Instead of storing textures, store local numbers of what material to use
	// The model would than provide what textures go into each number
	// For example say theres a model with two meshs, mesh1 has a material1, the mesh2 has a different texture so its would store a different material
	// TODO: Change how the materials are stored and perhaps store them on the model instead of mesh, so that the same mesh can be used with different models


	//std::vector<Texture*> textures;
	Material* material;


	void Draw(Shader* shader);

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
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
	void InitialiseQuad();
	void InitialiseDoubleSidedQuad();
	void InitialiseCube();

	void InitialiseFromFile(std::string filename);
	void InitialiseIndexFromFile(std::string filename, int i);
	void InitialiseFromAiMesh(std::string path, const aiScene* scene, aiMesh* mesh);
private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void GenAndBind();
	void Unbind();
	//TODO: Both the aiTextureType and the Texture::Type shouldn't be passed, make a dictionary or something and only pass one in
	static std::vector<Texture*> LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type);
	static void AddMaterialTextures(std::vector<Texture*>* textures, std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type);
};