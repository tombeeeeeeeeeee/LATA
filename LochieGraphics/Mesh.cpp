#include "Mesh.h"

#include <iostream>

Vertex::Vertex(glm::vec2 pos, glm::vec2 tex) :
	position(pos),
	texCoord(tex)
{
}

void Vertex::EnableAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(Vertex::position) / sizeof(Vertex::position.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// Texture
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(Vertex::texCoord) / sizeof(Vertex::texCoord.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) :
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0)
{
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	Initialise((int)vertices.size(), &vertices[0], (int)indices.size(), &indices[0]);
}

Mesh::Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, GLuint* indices) : Mesh()
{
	Initialise(vertexCount, vertices, indexCount, indices);
}

Mesh::Mesh() :
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0),
	max(glm::zero<glm::vec3>()),
	min(glm::zero<glm::vec3>())
{
}

Mesh::~Mesh()
{
	// TODO: Clean up 
	////std::cout << "A mesh was destroyed\n";
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &IBO);
}

void Mesh::Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, GLuint* indices)
{
	if (VAO != 0) {
		std::cout << "Error, trying to load a mesh on already loaded mesh!\n";
		return;
	}

	GenAndBind();

	// fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	Vertex::EnableAttributes();

	// bind indices if there are any
	if (indexCount > 0) {
		glGenBuffers(1, &IBO);

		// bind vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		// fill vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else {
		triCount = vertexCount / 3;
	}

	// Unbind buffers
	Unbind();
}

unsigned int Mesh::getTriCount() const
{
	return triCount;
}

GLuint Mesh::getVAO() const
{
	return VAO;
}

GLuint Mesh::getVBO() const
{
	return VBO;
}

GLuint Mesh::getIBO() const
{
	return IBO;
}

void Mesh::InitialiseQuad(float size, float offset)
{
	const unsigned int vertexCount = 6;
	Vertex vertices[vertexCount] = {
		{ {  size + offset,  size + offset }, { 1.0f, 1.0f } }, // Top right
		{ { -size + offset,  size + offset }, { 0.0f, 1.0f } }, // Top left
		{ { -size + offset, -size + offset }, { 0.0f, 0.0f } }, // Bottom left
		{ {  size + offset,  size + offset }, { 1.0f, 1.0f } }, // Top right
		{ { -size + offset, -size + offset }, { 0.0f, 0.0f } }, // Bottom left
		{ {  size + offset, -size + offset }, { 1.0f, 0.0f } } // Bottom right
	};
	Initialise(vertexCount, vertices);
}

void Mesh::Draw()
{
	// draw mesh
	glBindVertexArray(VAO);
	if (IBO != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	}

	Unbind();
}

void Mesh::GenAndBind()
{
	// Generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Bind vertex array
	glBindVertexArray(VAO);
}

void Mesh::Unbind()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
