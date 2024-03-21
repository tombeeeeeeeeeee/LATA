#pragma once
#include "Maths.h"

struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;

	Vertex(glm::vec4 pos, glm::vec4 nor, glm::vec2 tex);
	static void EnableAttributes();
	Vertex() {};
};