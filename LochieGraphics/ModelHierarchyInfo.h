#pragma once

#include "Transform.h"

#include <string>


class ModelHierarchyInfo
{
public:
	Transform transform;
	std::string name = "";

	// TODO: This is just so something actually owns these so that they can be deleted
	// TODO: Actually delete them
	std::vector<ModelHierarchyInfo*> children;
};
