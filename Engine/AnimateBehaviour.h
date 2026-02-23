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

	// TODO: Blend option should really be in the transition itself however it doesn't need to be with the way it is used
	bool blend;

	AnimateBehaviour(Animation* _animation, bool _loop, bool _blend);

	virtual void Enter(SceneObject* so);
	virtual void Update(SceneObject* so, float delta);
	virtual void Exit(SceneObject* so);

};

