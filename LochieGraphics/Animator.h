#pragma once

#include "Maths.h"

#include <vector>

// DON'T FORGET TO CHANGE ON SHADER IF CHANGING
constexpr auto MAX_BONES_ON_MODEL = 130;

class Animation;
class ModelHierarchyInfo;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

// TODO: With how some stuff is laid out almost feels like it would be worth having an 

class Animator
{
protected:
	// The model transformation for each bone, stored in bone index order.
	std::vector<glm::mat4> finalBoneMatrices;

	// The current animation that is playing.
	Animation* currentAnimation = nullptr;
	unsigned long long currentAnimationGUID = 0;
	
	// The current time into the animation.
	float currentTime = 0.f;
	
	Animator(toml::table table);

public:
	enum Type : unsigned int {
		base,
		blended,
		directional2dAnimator,
	};

	Animator();
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
	const std::vector<glm::mat4>& getFinalBoneMatrices() const;

	std::vector<glm::mat4>& getNonConstFinalBoneMatrices();

	virtual toml::table Serialise(unsigned long long GUID) const;

	virtual void GUI();

	virtual Type getType() const;

	static Animator* Load(toml::table table);

protected:

	void BaseGUI();
};

