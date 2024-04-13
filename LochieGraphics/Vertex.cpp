#include "Vertex.h"

#include "Graphics.h"


Vertex::Vertex(glm::vec4 pos, glm::vec4 nor, glm::vec2 tex, glm::vec3 tan, glm::vec3 bit) :
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
	glVertexAttribPointer(0, sizeof(Vertex::position)  / sizeof(Vertex::position.x),  GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, sizeof(Vertex::normal)    / sizeof(Vertex::normal.x),    GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	// Texture
	glVertexAttribPointer(2, sizeof(Vertex::texCoord)  / sizeof(Vertex::texCoord.x),  GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);
	// Tangents
	glVertexAttribPointer(3, sizeof(Vertex::tangent)   / sizeof(Vertex::tangent.x),   GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);
	// Bi Tangents
	glVertexAttribPointer(4, sizeof(Vertex::biTangent) / sizeof(Vertex::biTangent.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));
	glEnableVertexAttribArray(4);
	// Bone IDs
	glVertexAttribIPointer(5, sizeof(Vertex::boneIDs)  / sizeof(Vertex::boneIDs[0]),  GL_INT,             sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
	glEnableVertexAttribArray(5);
	// Bone weights
	glVertexAttribPointer(6, sizeof(Vertex::weights)   / sizeof(Vertex::weights[0]),  GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
	glEnableVertexAttribArray(6);
}