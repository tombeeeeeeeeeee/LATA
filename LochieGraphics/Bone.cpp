#include "Bone.h"

#include "AssimpMatrixToGLM.h"

#include <iostream>

Bone::Bone(const std::string& _name, int _ID, const aiNodeAnim* channel) :
	name(_name),
	ID(_ID),
	localTransform(1.0f)
{
	if (channel->mNumPositionKeys != channel->mNumRotationKeys || channel->mNumRotationKeys != channel->mNumScalingKeys) {
		std::cout << "Error, failed to load bone " << name << ", Animation will likely be broken\n";
		//TODO: See if there is a way through assimp to ensure there is the same amount of keys
		// TODO: Also ensure the times are the same for each key
		return;
	}
	unsigned int keyCount = channel->mNumPositionKeys;

	keys.reserve(keyCount);
	for (unsigned int i = 0; i < keyCount; i++)
	{
		keys.push_back(Key {
			AssimpVecToGLM(channel->mPositionKeys[i].mValue),  // Position
			AssimpQuatToGLM(channel->mRotationKeys[i].mValue), // Rotation
			AssimpVecToGLM(channel->mScalingKeys[i].mValue),   // Scale
			(float)channel->mPositionKeys[i].mTime             // Timestamp
			});
	}
}

void Bone::Update(float animationTime)
{
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

int Bone::getIndex(float animationTime) const
{
	for (int i = 1; i < keys.size(); i++)
	{
		if (animationTime < keys[i].timeStamp) {
			return i - 1;
		}
	}
	return 0;
	//TODO: No asserts, here and the other get index
	//assert(0); 
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff; // Scale factor
}

glm::mat4 Bone::Interpolate(float animationTime)
{
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 scale;

	if (keys.size() == 1) {
		translation = glm::translate(glm::mat4(1.0f), keys[0].position);
		rotation = glm::mat4_cast(glm::normalize(keys[0].orientation));
		scale = glm::scale(glm::mat4(1.0f), keys[0].scale);

		return translation * rotation * scale;
	}

	int p0Index = getIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(keys[p0Index].timeStamp, keys[p1Index].timeStamp, animationTime);

	// TODO: Don't need to have three matrices and could just adjust the an existing final matrix
	glm::vec3 finalPosition = glm::mix(keys[p0Index].position, keys[p1Index].position, scaleFactor);
	translation = glm::translate(glm::mat4(1.0f), finalPosition);

	glm::quat finalRotation = glm::slerp(keys[p0Index].orientation, keys[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	rotation = glm::mat4_cast(finalRotation);

	glm::vec3 finalScale = glm::mix(keys[p0Index].scale, keys[p1Index].scale, scaleFactor);
	scale = glm::scale(glm::mat4(1.0f), finalScale);

	return translation * rotation * scale;
}