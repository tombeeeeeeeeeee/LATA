#pragma once

#include "Animation.h"

#include "Serialisation.h"

class Animator
{
protected:
	// The model transformation for each bone, stored in bone index order.
	std::vector<glm::mat4> finalBoneMatrices;

	// The current animation that is playing.
	Animation* currentAnimation = nullptr;
	
	// The current time into the animation.
	float currentTime = 0.f;

public:
	Animator() {};
	/// <summary>
	/// Sets a starting animation.
	/// </summary>
	/// <param name="animation">The animation to play</param>
	Animator(Animation* animation);

	/// <summary>
	/// Updates the animation based on the time passed, this will recalculate all of the bone transformations.
	/// </summary>
	/// <param name="delta">time passed</param>
	virtual void UpdateAnimation(float delta);

	/// <summary>
	/// Changes the current animation to the given one, resets the current time back to zero.
	/// </summary>
	/// <param name="animation">The new animation to play</param>
	void PlayAnimation(Animation* animation);

	/// <summary>
	/// Calculates the bone transform of a node and it's children.
	/// </summary>
	/// <param name="node">The node to calculate the transform of</param>
	/// <param name="parentTransform">The parent transformation</param>
	virtual void CalculateBoneTransform(const ModelHierarchyInfo* node, glm::mat4 parentTransform);

	/// <summary>
	/// Returns a reference to the bone matrices.
	/// </summary>
	/// <returns>const reference vector of the bone transforms</returns>
	const std::vector<glm::mat4>& getFinalBoneMatrices();

	toml::table Serialise(unsigned long long GUID) const;
};

