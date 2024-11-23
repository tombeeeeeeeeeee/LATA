#pragma once
#include "Behaviour.h"

class BlendedAnimator;
class Animation;

class AnimateBehaviour : public Behaviour
{
public:

	BlendedAnimator* animator;

	Animation* animation;

	bool loop;

	AnimateBehaviour(Animation* animation, bool loop);

	virtual void Enter(SceneObject* so);
	virtual void Update(SceneObject* so, float delta);
	virtual void Exit(SceneObject* so);

};

