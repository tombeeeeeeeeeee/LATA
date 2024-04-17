#include "Bone.h"

#include "AssimpMatrixToGLM.h"

Bone::Bone(const std::string& _name, int _ID, const aiNodeAnim* channel) :
	name(_name),
	ID(_ID),
	localTransform(1.0f)
{
	positions.reserve(channel->mNumPositionKeys);
	for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; positionIndex++)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data = {
			AssimpVecToGLM(aiPosition),
			timeStamp
		};
		positions.push_back(data);
	}

	rotations.reserve(channel->mNumRotationKeys);
	for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data = {
			AssimpQuatToGLM(aiOrientation),
			timeStamp
		};
		rotations.push_back(data);
	}

	scales.reserve(channel->mNumScalingKeys);
	for (int keyIndex = 0; keyIndex < channel->mNumScalingKeys; ++keyIndex)
	{
		aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = (float)channel->mScalingKeys[keyIndex].mTime;
		KeyScale data = {
			AssimpVecToGLM(scale),
			timeStamp
		};
		scales.push_back(data);
	}
}

void Bone::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScaling(animationTime);
	localTransform = translation * rotation * scale;
}

glm::mat4 Bone::getLocalTransform() const
{
	return localTransform;
}

std::string Bone::getBoneName() const
{
	return name;
}

int Bone::getBoneID() const
{
	return ID;
}

int Bone::getPositionIndex(float animationTime) const
{
	for (int index = 1; index < positions.size(); index++)
	{
		if (animationTime < positions[index].timeStamp) {
			return index - 1;
		}
	}
	return 0;
	//assert(0); //TODO: No asserts, here and the other get index
}

int Bone::getRotationIndex(float animationTime) const
{
	for (int index = 1; index < rotations.size(); index++)
	{
		if (animationTime < rotations[index].timeStamp) {
			return index - 1;
		}
	}
	return 0;
	//assert(0); // TODO: remove
}

int Bone::getScaleIndex(float animationTime) const
{
	for (int index = 1; index < scales.size(); index++)
	{
		if (animationTime < scales[index].timeStamp) {
			return index - 1;
		}
	}
	return 0;
	//assert(0); // TODO: remove
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff; // Scale factor
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (positions.size() == 1) {
		return glm::translate(glm::mat4(1.0f), positions[0].position);
	}

	int p0Index = getPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (rotations.size() == 1)
	{
		auto rotation = glm::normalize(rotations[0].orientation);
		return glm::mat4_cast(rotation);
	}

	int p0Index = getRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
	if (scales.size() == 1) {
		return glm::scale(glm::mat4(1.0f), scales[0].scale);
	}

	int p0Index = getScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}
