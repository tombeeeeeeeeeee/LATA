#pragma once
#include "Maths.h"

struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;
	glm::vec3 biTangent;

	// TODO: Don't default these to 0
	Vertex(glm::vec4 pos = { 0.f, 0.f, 0.f, 0.f }, glm::vec4 nor = { 0.f, 0.f, 0.f, 0.f }, glm::vec2 tex = { 0.f, 0.f }, glm::vec3 tan = { 0.f, 0.f, 0.f }, glm::vec3 bit = { 0.f, 0.f, 0.f });
	static void EnableAttributes();
};