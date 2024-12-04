#include "AnimateBehaviour.h"

#include "SceneObject.h"
#include "BlendedAnimator.h"

AnimateBehaviour::AnimateBehaviour(Animation* _animation, bool _loop, bool _blend) :
	animation(_animation),
	loop(_loop),
	blend(_blend)
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

	if (blend) {
		animator->lerpAmount = 1.0f;
	}

}

void AnimateBehaviour::Update(SceneObject* so, float delta)
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

	if (blend) {
		animator->lerpAmount = fmaxf(0.0f, animator->lerpAmount - delta * 5);
	}
	else {
		animator->lerpAmount = 0.0f;
	}
}

void AnimateBehaviour::Exit(SceneObject* so)
{
}
