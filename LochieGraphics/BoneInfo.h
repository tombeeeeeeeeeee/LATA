#pragma once

#include "Maths.h"

struct BoneInfo
{
	// Bone ID
	unsigned int ID;

	// Offset matrix transforms vertex from model space to bone space
	glm::mat4 offset;
};