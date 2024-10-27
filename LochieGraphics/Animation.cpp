#include "Animation.h"

#include "BoneInfo.h"
#include "Model.h"

#include "Utilities.h"

#include "EditorGUI.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

Animation::Animation(const std::string& animationPath, Model* _model) :
	model(_model),
	modelGUID(_model->GUID)
{
	// TODO: Can I just reuse like 1 importer?
	Assimp::Importer importer;
	// TODO: what flags should be set here
	// TODO: Properties like this are also set in Model, they should be ensured to match
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	
	// If either the scene or the root node is null, then the animation has failed to load
	if (!(scene && scene->mRootNode)) {
		std::cout << "Error: Failed to load animation at: " << animationPath << "\n";
		return;
	}

	// Get animation info from the aiAnimation in the scene
	aiAnimation* animation = scene->mAnimations[0];
	duration = (float)animation->mDuration;
	ticksPerSecond = (float)animation->mTicksPerSecond;
	
	//aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	//globalTransformation = globalTransformation.Inverse();

	//rootNode = &(new SceneObject())->transform;
	//SceneManager::scene->sceneObjects.push_back(rootNode->getSceneObject());
	
	//ReadHierarchyData(rootNode, scene->mRootNode);
	ReadMissingBones(animation, model);
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

const ModelHierarchyInfo* Animation::getRootNode() const
{
	return &model->root;
}

void Animation::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Animation##" + tag).c_str())) {
		ImGui::Indent();
		ImGui::DragFloat(("Duration##" + tag).c_str(), &duration);
		ImGui::DragFloat(("Ticks Per Second##" + tag).c_str(), &ticksPerSecond);
		
		if (ImGui::CollapsingHeader(("Bones##" + tag).c_str())) {
			ImGui::Indent();
			for (size_t i = 0; i < bones.size(); i++)
			{
				bones[i].GUI();
			}
			ImGui::Unindent();
		}

		ImGui::BeginDisabled();
		std::string modelName = model->getDisplayName();
		ImGui::InputText(("Model##" + tag).c_str(), &modelName);
		ImGui::EndDisabled();
		ImGui::Unindent();
	}
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model* model)
{
	std::unordered_map<std::string, BoneInfo>& newBoneInfoMap = model->boneInfoMap;

	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (newBoneInfoMap.find(boneName) == newBoneInfoMap.end()) {
			newBoneInfoMap[boneName].ID = (unsigned int)newBoneInfoMap.size() + 1;
		}
		bones.push_back(Bone(channel->mNodeName.data, newBoneInfoMap[channel->mNodeName.data].ID, channel));
	}
}
