#pragma once

#include "Maths.h"

#include "Texture.h"
#include "Shader.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;

	Vertex(glm::vec4 pos, glm::vec4 nor, glm::vec2 tex);
	Vertex() {};
};
class Mesh
{
private:
	unsigned int triCount;
	unsigned int VAO, VBO, IBO;

	std::vector<Texture*> textures;
public:

	void Draw(Shader& shader);

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);
	Mesh();
	~Mesh();

	// Pre set
	void InitialiseQuad();
	void InitialiseCube();

	void InitialiseFromFile(const char* filename);
	void InitialiseIndexFromFile(const char* filename, int i);
	void InitialiseFromAiMesh(std::string path, const aiScene* scene, aiMesh* mesh);
private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void Unbind();
	static std::vector<Texture*> LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type);
};