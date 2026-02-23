#include "Bone.h"

#include "ExtraEditorGUI.h"
#include "AssimpMatrixToGLM.h"

#include "assimp/anim.h"

#include <iostream>

Bone::Bone(const std::string& _name, int _ID, const aiNodeAnim* channel) :
	name(_name),
	ID(_ID),
	localTransform(1.0f)
{
	posKeys.reserve(channel->mNumPositionKeys);
	for (unsigned int i = 0; i < channel->mNumPositionKeys; i++)
	{
		posKeys.push_back(PosKey{ AssimpVecToGLM(channel->mPositionKeys[i].mValue), (float)channel->mPositionKeys[i].mTime});
	}
	for (unsigned int i = 0; i < channel->mNumRotationKeys; i++)
	{
		rotKeys.push_back(RotKey{ AssimpQuatToGLM(channel->mRotationKeys[i].mValue), (float)channel->mRotationKeys[i].mTime });
	}
	for (unsigned int i = 0; i < channel->mNumScalingKeys; i++)
	{
		scaleKeys.push_back(ScaleKey{ AssimpVecToGLM(channel->mScalingKeys[i].mValue), (float)channel->mScalingKeys[i].mTime });
	}

	//if (channel->mNumPositionKeys != channel->mNumRotationKeys || channel->mNumRotationKeys != channel->mNumScalingKeys) {
	//	std::cout << "Error, failed to load bone " << name << ", Animation will likely be broken\n";
	//	//TODO: See if there is a way through assimp to ensure there is the same amount of keys
	//	// TODO: Also ensure the times are the same for each key
	//	return;
	//}
	//unsigned int keyCount = channel->mNumPositionKeys;

	//keys.reserve(keyCount);
	//for (unsigned int i = 0; i < keyCount; i++)
	//{
	//	keys.push_back(Key {
	//		AssimpVecToGLM(channel->mPositionKeys[i].mValue),  // Position
	//		AssimpQuatToGLM(channel->mRotationKeys[i].mValue), // Rotation
	//		AssimpVecToGLM(channel->mScalingKeys[i].mValue),   // Scale
	//		(float)channel->mPositionKeys[i].mTime             // Timestamp
	//		});
	//}
}

void Bone::Update(float animationTime)
{
	if (glm::isnan(localTransform[1][1])) {
		std::cout << "Nan-i?!\n";
	}
	localTransform = Interpolate(animationTime);
}

const glm::mat4& Bone::getLocalTransform() const
{
	return localTransform;
}

const std::string& Bone::getBoneName() const
{
	return name;
}

int Bone::getBoneID() const
{
	return ID;
}

int Bone::getPosIndex(float animationTime) const
{
	for (int i = 1; i < posKeys.size(); i++)
	{
		if (animationTime < posKeys[i].timeStamp) {
			return i - 1;
		}
	}
	return 0;
}

int Bone::getRotIndex(float animationTime) const
{
	for (int i = 1; i < rotKeys.size(); i++)
	{
		if (animationTime < rotKeys[i].timeStamp) {
			return i - 1;
		}
	}
	return 0;

}

int Bone::getScaleIndex(float animationTime) const
{
	for (int i = 1; i < scaleKeys.size(); i++)
	{
		if (animationTime < scaleKeys[i].timeStamp) {
			return i - 1;
		}
	}
	return 0;
}

//int Bone::getIndex(float animationTime) const
//{
//	for (int i = 1; i < keys.size(); i++)
//	{
//		if (animationTime < keys[i].timeStamp) {
//			return i - 1;
//		}
//	}
//	return 0;
//	//TODO: No asserts, here and the other get index
//	//assert(0); 
//}

void Bone::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	//if (ImGui::CollapsingHeader((name + "##" + tag).c_str())) {
		//ImGui::Indent();
	ImGui::InputText(("Name##" + tag).c_str(), &name);
	ExtraEditorGUI::Mat4Input(tag, &localTransform);
	ImGui::InputInt(("ID##" + tag).c_str(), &ID);
	if (ImGui::CollapsingHeader(("Keys##" + tag).c_str())) {
		ImGui::Indent();
		// TODO: Fix
		for (size_t i = 0; i < posKeys.size(); i++)
		{
			std::string keyTag = Utilities::PointerToString(&posKeys[i]);
			if (!ImGui::CollapsingHeader(("Pos Key " + std::to_string(i) + "##" + keyTag).c_str())) {
				continue;
			}
			ImGui::DragFloat3(("Pos##" + keyTag).c_str(), &posKeys[i].value.x);
			ImGui::DragFloat(("Time stamp##" + keyTag).c_str(), &posKeys[i].timeStamp);

		}
		for (size_t i = 0; i < rotKeys.size(); i++)
		{
			std::string keyTag = Utilities::PointerToString(&rotKeys[i]);
			if (!ImGui::CollapsingHeader(("Rot Key " + std::to_string(i) + "##" + keyTag).c_str())) {
				continue;
			}
			ImGui::DragFloat4(("Quat##" + keyTag).c_str(), &rotKeys[i].orientation[0]);
			ImGui::BeginDisabled();
			glm::vec3 rot = glm::eulerAngles(rotKeys[i].orientation);
			ImGui::DragFloat3(("Rot##" + keyTag).c_str(), &rot.x);
			ImGui::EndDisabled();
			ImGui::DragFloat(("Time stamp##" + keyTag).c_str(), &rotKeys[i].timeStamp);

		}
		for (size_t i = 0; i < scaleKeys.size(); i++)
		{
			std::string keyTag = Utilities::PointerToString(&scaleKeys[i]);
			if (!ImGui::CollapsingHeader(("Scale Key " + std::to_string(i) + "##" + keyTag).c_str())) {
				continue;
			}
			ImGui::DragFloat3(("Scale##" + keyTag).c_str(), &scaleKeys[i].scale.x);
			ImGui::DragFloat(("Time stamp##" + keyTag).c_str(), &scaleKeys[i].timeStamp);

		}
		ImGui::Unindent();
	}
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
	if (lastTimeStamp == nextTimeStamp) { return 0.0f; }
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff; // Scale factor
}

glm::mat4 Bone::Interpolate(float animationTime)
{
	// TODO: Don't need to have three matrices and could just adjust the an existing final matrix
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 scale;


	int pos0Index = getPosIndex(animationTime);
	int pos1Index = glm::min(pos0Index + 1, (int)posKeys.size() - 1);
	float posScaleFactor = getScaleFactor(posKeys[pos0Index].timeStamp, posKeys[pos1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(posKeys[pos0Index].value, posKeys[pos1Index].value, posScaleFactor);
	translation = glm::translate(glm::mat4(1.0f), finalPosition);

	int rot0Index = getRotIndex(animationTime);
	int rot1Index = glm::min(rot0Index + 1, (int)rotKeys.size() - 1);
	float rotScaleFactor = getScaleFactor(rotKeys[rot0Index].timeStamp, rotKeys[rot1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(rotKeys[rot0Index].orientation, rotKeys[rot1Index].orientation, rotScaleFactor);
	finalRotation = glm::normalize(finalRotation);
	rotation = glm::mat4_cast(finalRotation);
	
	int scale0Index = getScaleIndex(animationTime);
	int scale1Index = glm::min(scale0Index + 1, (int)scaleKeys.size() - 1);
	float scaleScaleFactor = getScaleFactor(scaleKeys[scale0Index].timeStamp, scaleKeys[scale1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(scaleKeys[scale0Index].scale, scaleKeys[scale1Index].scale, scaleScaleFactor);
	scale = glm::scale(glm::mat4(1.0f), finalScale);

	return translation * rotation * scale;
}