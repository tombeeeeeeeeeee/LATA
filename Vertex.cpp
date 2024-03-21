#include "Vertex.h"

#include "Graphics.h"


Vertex::Vertex(glm::vec4 pos, glm::vec4 nor, glm::vec2 tex) :
	position(pos),
	normal(nor),
	texCoord(tex)
{
}

void Vertex::EnableAttributes()
{
	// Position
	glVertexAttribPointer(0, sizeof(Vertex::position) / sizeof(Vertex::position.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, sizeof(Vertex::normal) / sizeof(Vertex::normal.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	// Texture
	glVertexAttribPointer(2, sizeof(Vertex::texCoord) / sizeof(Vertex::texCoord.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);
}

