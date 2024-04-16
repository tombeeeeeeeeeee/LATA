#pragma once

#include "Maths.h"
#include "assimp/scene.h"


#include <vector>
#include <string>

struct KeyPosition {
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation {
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale {
	glm::vec3 scale;
	float timeStamp;
};

class Bone {
private:
	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;

	int numPositions;
	int numRotations;
	int numScales;

	glm::mat4 localTransform;
	std::string name;
	int ID;

public:
	Bone(const std::string& _name, int _ID, const aiNodeAnim* channel);
	void Update(float animationTime);
	glm::mat4 getLocalTransform() const;
	std::string getBoneName() const;
	int getBoneID() const;

	int getPositionIndex(float animationTime) const;
	int getRotationIndex(float animationTime) const;
	int getScaleIndex(float animationTime) const;

private:


	float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 InterpolateRotation(float animationTime);
	glm::mat4 InterpolateScaling(float animationTime);

};