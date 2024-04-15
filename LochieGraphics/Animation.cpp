#include "Animation.h"

#include "AssimpMatrixToGLM.h"

#include <assimp/Importer.hpp>

Animation::Animation(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	// TODO: what flags should be set here
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	ReadHeirarchyData(rootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);
}

Bone* Animation::FindBone(const std::string& name)
{
	// TODO: A lot of iterators are used just to find stuff, see if it is something to be using more
	auto search = std::find_if(bones.begin(), bones.end(), [&](const Bone& bone) {
		return bone.getBoneName() == name;
		});
	if (search == bones.end()) {
		return nullptr;
	}

	return &(*search);
}

float Animation::getTicksPerSecond() const
{
	return ticksPerSecond;
}

float Animation::getDuration() const
{
	return duration;
}

const AssimpNodeData& Animation::getRootNode()
{
	return rootNode;
}

const std::unordered_map<std::string, BoneInfo>& Animation::getBoneIDMap()
{
	return boneInfoMap;
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& newBoneInfoMap = model.boneInfoMap;

	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (newBoneInfoMap.find(boneName) == newBoneInfoMap.end()) {
			newBoneInfoMap[boneName].id = boneInfoMap.size() + 1;
		}
		bones.push_back(Bone(channel->mNodeName.data, newBoneInfoMap[channel->mNodeName.data].id, channel));
	}
	boneInfoMap = newBoneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpMatrixToGLM(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHeirarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
