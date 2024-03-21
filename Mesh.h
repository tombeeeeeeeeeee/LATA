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
	std::vector<Texture*> textures;

	void Draw(Shader& shader);

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
	Mesh();
	~Mesh();

	// Pre set
	//TODO: add more and customisable, specifically sphere
	void InitialiseQuad();
	void InitialiseDoubleSidedQuad();
	void InitialiseCube();

	void InitialiseFromFile(const char* filename);
	void InitialiseIndexFromFile(const char* filename, int i);
	void InitialiseFromAiMesh(std::string path, const aiScene* scene, aiMesh* mesh);
private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void GenAndBind();
	void Unbind();
	//TODO: Both the aiTextureType and the Texture::Type shouldn't be passed, make a dictionary or something and only pass one in
	static std::vector<Texture*> LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type);
	void AddMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type);
};