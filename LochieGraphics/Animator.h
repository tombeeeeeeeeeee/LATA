#pragma once

#include "Animation.h"

class Animator
{
private:

	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;

public:
	Animator() {};
	Animator(Animation* animation);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* animation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> getFinalBoneMatrices();

};

