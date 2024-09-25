#pragma once
#include "Condition.h"

class DistanceAndLOSCondition : Condition
{
	bool IsTrueWhenLessThan = false;
	float range = 0.0f;
	DistanceAndLOSCondition(bool lessThan, float _range) : IsTrueWhenLessThan(lessThan), range(_range) {};
	virtual bool IsTrue(SceneObject* so);
};

