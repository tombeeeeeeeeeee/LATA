#pragma once

#include "Transform.h"

#include <string>

class Model;

class ModelHierarchyInfo
{
public:
	Transform transform;
	std::string name = "";

	// TODO: Use
	std::vector<unsigned int> meshes;

	// TODO: This is just so something actually owns these so that they can be deleted
	// TODO: Actually delete them
	std::vector<ModelHierarchyInfo*> children;

	void GUI();

	// TODO: Rename, is model not global
	bool GlobalMatrixOfMesh(unsigned int meshIndex, Model* model, glm::mat4& matrix);
};
