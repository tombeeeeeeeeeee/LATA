#pragma once

#include "Transform.h"

#include <string>
#include <unordered_map>

class Model;
struct BoneInfo;

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

	void GUI(bool header = true, std::unordered_map<std::string, BoneInfo>* boneInfoMap = nullptr);

	bool ModelMatrixOfMesh(unsigned int meshIndex, glm::mat4& matrix);
};
