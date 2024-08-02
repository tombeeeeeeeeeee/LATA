#include "Vertex.h"

#include "Graphics.h"


Vertex::Vertex(glm::vec3 pos, glm::vec3 nor, glm::vec2 tex, glm::vec3 tan, glm::vec3 bit) :
	position(pos),
	normal(nor),
	texCoord(tex),
	tangent(tan),
	biTangent(bit)
{
}

void Vertex::EnableAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(Vertex::position)  / sizeof(Vertex::position.x),  GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(Vertex::normal)    / sizeof(Vertex::normal.x),    GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// Texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, sizeof(Vertex::texCoord)  / sizeof(Vertex::texCoord.x),  GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	// Tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, sizeof(Vertex::tangent)   / sizeof(Vertex::tangent.x),   GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// Bi Tangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, sizeof(Vertex::biTangent) / sizeof(Vertex::biTangent.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));
	// Bone IDs
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, MAX_BONES_PER_VERTEX,                                   GL_INT,             sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
	// Bone weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, MAX_BONES_PER_VERTEX,                                    GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
}