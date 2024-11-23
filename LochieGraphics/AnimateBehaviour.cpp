#include "AnimateBehaviour.h"

#include "SceneObject.h"
#include "BlendedAnimator.h"

AnimateBehaviour::AnimateBehaviour(Animation* _animation, bool _loop) :
	animation(_animation),
	loop(_loop)
{
}

void AnimateBehaviour::Enter(SceneObject* so)
{
	Animator* temp = so->animator();
	if (!temp) {
		// TODO: Error for no animator
		animator = nullptr;
		return;
	}
	if (temp->getType() != Animator::Type::blended) {
		// TODO: Error here, this only works for blended animators
		animator = nullptr;
		return;
	}
	animator = (BlendedAnimator*)temp;
	animator->SwitchToAnimation(animation);
	animator->loopCurrent = loop;
}

void AnimateBehaviour::Update(SceneObject* so, float delta)
{
}

void AnimateBehaviour::Exit(SceneObject* so)
{
}
