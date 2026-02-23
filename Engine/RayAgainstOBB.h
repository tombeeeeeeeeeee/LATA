#pragma once

#include "Maths.h"

namespace RayAgainstOBB
{
	// TODO: Might not be working properly, test
	bool RayAgainstOBB(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabbMin, glm::vec3 aabbMax, glm::mat4 model, float& intersectionDistance);
};

