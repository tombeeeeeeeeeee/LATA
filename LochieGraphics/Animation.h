#pragma once

#include "Bone.h"
#include "Model.h"
#include "NodeData.h"

#include "assimp/scene.h"

#include <string>

class Animation
{
private:
	float duration = 0;
	float ticksPerSecond = 0.f;
	std::vector<Bone> bones = {};
	NodeData rootNode;
	std::unordered_map<std::string, BoneInfo> boneInfoMap;

public:
	Animation() {};

	Animation(const std::string& animationPath, Model* model);
	
	Bone* FindBone(const std::string& name);
	
	float getTicksPerSecond() const;

	float getDuration() const;

	const NodeData& getRootNode() const;

	const std::unordered_map<std::string, BoneInfo>& getBoneIDMap() const;

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);

	void ReadHierarchyData(NodeData& dest, const aiNode* src);
};

