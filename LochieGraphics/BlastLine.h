#pragma once
#include "Maths.h"

struct BlastLine
{
	float lifeSpan;
	float timeElapsed;
	glm::vec3 colour;
	glm::vec3 startPosition;
	glm::vec3 endPosition;
};