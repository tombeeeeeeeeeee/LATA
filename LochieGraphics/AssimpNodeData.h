#pragma once

#include "Maths.h"

#include <string>
#include <vector>

// TODO: Rename class
struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	std::vector<AssimpNodeData> children;
};