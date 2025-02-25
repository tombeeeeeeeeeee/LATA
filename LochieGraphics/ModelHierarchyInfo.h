#pragma once

#include "Transform.h"

#include <string>
#include <unordered_map>

class ModelHierarchyInfo
{
public:
	Transform transform;
	std::string name = "";

	std::vector<unsigned int> meshes;

	// TODO: This is just so something actually owns these so that they can be deleted
	// TODO: Actually delete them
	std::vector<ModelHierarchyInfo*> children;

	bool ModelHierarchyInfoOfMesh(unsigned int meshIndex, ModelHierarchyInfo** info);
};
