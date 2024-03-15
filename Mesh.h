#pragma once

#include "glm.hpp"
//TODO: make mesh renderer class to store textures and stuff
class Mesh
{
private:

	unsigned int triCount;
	unsigned int VAO, VBO, ibo;
public:

	Mesh();
	~Mesh();

	struct Vertex {
		glm::vec4 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, unsigned int* indices = nullptr);

	void InitialiseQuad();

	void InitialiseCube();

	void InitialiseFromFile(const char* filename);

	void Draw();


};

