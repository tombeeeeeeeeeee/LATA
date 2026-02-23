#pragma once
#include "Condition.h"

// Specifically made with blended animators in mind
class AnimationFinishedCondition : public Condition
{
public:
	enum class Check {
		current,
		otherCurrent
	};
	Check check;

	AnimationFinishedCondition(Check check = Check::current);

	virtual bool IsTrue(SceneObject* so) const override;

};

