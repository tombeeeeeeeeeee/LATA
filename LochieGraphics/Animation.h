#pragma once

#include "Bone.h"
#include "Model.h"

#include "assimp/scene.h"

#include <string>

struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	std::vector<AssimpNodeData> children;
};

class Animation
{
private:
	float duration = 0;
	float ticksPerSecond;
	std::vector<Bone> bones;
	AssimpNodeData rootNode;
	std::unordered_map<std::string, BoneInfo> boneInfoMap;

public:
	Animation() {};

	Animation(const std::string& animationPath, Model* model);
	
	Bone* FindBone(const std::string& name);
	
	float getTicksPerSecond() const;

	float getDuration() const;

	const AssimpNodeData& getRootNode();

	const std::unordered_map<std::string, BoneInfo>& getBoneIDMap();

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);

	void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};

