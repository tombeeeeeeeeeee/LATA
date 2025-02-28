#pragma once
#include "Graphics.h"

#include "Maths.h"

#include <vector>
#include <unordered_map>



struct Vertex {
	glm::vec2 position;
	glm::vec2 texCoord;

	// TODO: Don't default these to 0
	Vertex(glm::vec2 pos = { 0.f, 0.f }, glm::vec2 tex = { 0.f, 0.f });
	static void EnableAttributes();
};

class Mesh
{
public:
	glm::vec3 max, min;
private:
	unsigned int triCount;
	GLuint VAO, VBO, IBO;


public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);


	static int aiLoadFlag;

	// Not a GUID, this is the material index for the belonging model
	int materialID = 0;

	unsigned long long GUID;

	void Draw();

	Mesh();
	~Mesh();

	void InitialiseQuad(float size = 0.5f, float offset = 0.0f);

	unsigned int getTriCount() const;
	GLuint getVAO() const;
	GLuint getVBO() const;
	GLuint getIBO() const;

private:
	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, GLuint* indices = nullptr);

	void GenAndBind();
	// Should be static?
	void Unbind();
};