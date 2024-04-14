#pragma once
#include "Maths.h"

constexpr int MAX_BONES_PER_VERTEX = 4;


struct Vertex {

	// TODO: Make these vec3s
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;
	glm::vec3 biTangent;

	
	int boneIDs[MAX_BONES_PER_VERTEX] = { -1, -1, -1, -1 };
	float weights[MAX_BONES_PER_VERTEX] = { 0.f, 0.f, 0.f, 0.f};

	// TODO: Don't default these to 0
	Vertex(glm::vec4 pos = { 0.f, 0.f, 0.f, 0.f }, glm::vec4 nor = { 0.f, 0.f, 0.f, 0.f }, glm::vec2 tex = { 0.f, 0.f }, glm::vec3 tan = { 0.f, 0.f, 0.f }, glm::vec3 bit = { 0.f, 0.f, 0.f });
	static void EnableAttributes();
};