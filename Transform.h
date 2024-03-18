#pragma once
#include "Maths.h"
struct Transform
{
	glm::vec3 position = { 0.f, 0.f, 0.f };
	glm::vec3 rotation = { 0.f, 0.f, 0.f };
	float scale = 1.0f;
};

