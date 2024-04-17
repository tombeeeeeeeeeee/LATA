#pragma once

#include "Maths.h"

#include <string>
#include <vector>

// TODO: Rename class
struct NodeData {
	glm::mat4 transformation;
	std::string name;
	std::vector<NodeData> children;
};