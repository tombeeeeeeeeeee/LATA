#include "AnimationFinishedCondition.h"

#include "SceneObject.h"
#include "BlendedAnimator.h"
#include "Animation.h"

AnimationFinishedCondition::AnimationFinishedCondition(Check _check) :
	check(_check)
{
}

bool AnimationFinishedCondition::IsTrue(SceneObject* so) const
{
	Animator* animator = so->animator();
	if (!animator) {
		// TODO: Warning
		return false;
	}
	if (animator->getType() != Animator::Type::blended) {
		// TODO: Warning
		return false;
	}
	BlendedAnimator* blendedAnimator = (BlendedAnimator*)animator;
	if (check == Check::current) {
		return blendedAnimator->getAnimation()->getDuration() == blendedAnimator->getTime();
	}
	//else if (check == Check::otherCurrent)
	else {
		return blendedAnimator->getOtherAnimation()->getDuration() == blendedAnimator->getOtherTime();
	}
}
