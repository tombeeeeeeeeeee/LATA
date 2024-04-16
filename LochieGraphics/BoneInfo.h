#pragma once

#include "Maths.h"

struct BoneInfo
{
	int id;

	// Offset matrix transforms vertex from model space to bone space
	glm::mat4 offset;
};