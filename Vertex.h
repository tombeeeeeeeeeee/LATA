#pragma once
#include "Maths.h"

struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;
	glm::vec3 biTangent;

	// TODO: Don't default these to 0
	Vertex(glm::vec4 pos, glm::vec4 nor, glm::vec2 tex, glm::vec3 tan = { 0, 0, 0 }, glm::vec3 bit = { 0, 0, 0 });
	static void EnableAttributes();
	Vertex() {};
};