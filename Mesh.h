#pragma once

#include "Maths.h"

#include "Texture.h"

#include "Shader.h"
//TODO: make mesh renderer class to store textures and stuff
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

public:
	std::vector<Texture*> textures;

	//TODO: figure out to remove
	std::vector<unsigned int> indices;


	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, unsigned int* indices = nullptr);
	Mesh();
	~Mesh();




	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, unsigned int* indices = nullptr);

	void InitialiseQuad();

	void InitialiseCube();

	void InitialiseFromFile(const char* filename);

	void Draw(Shader& shader);

private:
	void Unbind();
};