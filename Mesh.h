#pragma once
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

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
	// Instead of storing textures, store local numbers of what texture to use
	// The model would than provide what textures go into each number
	// For example say theres a model with two meshs, mesh1 has a diffuse and specular, and those get stored as 1 and 2, the mesh2 has a different diffuse but same specular its material would get stored as 3 and 2
	// The model would make a material class with the materials in the right order
	// This would enable the mesh it self to not have to store the textures, but when with a model it could? and they could be switched eaiser?
	// A mesh drawn on it's own wouldn't have any textures assigned to it, it would have to be drawn via a model class to have textures

	std::vector<Texture*> textures;

	void Draw(Shader* shader);

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
	Mesh();
	~Mesh();

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
	void AddMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type);
};