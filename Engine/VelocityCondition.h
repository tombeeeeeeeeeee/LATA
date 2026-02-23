#pragma once

#include "Condition.h"

class VelocityCondition : public Condition
{
public:
	float minVel;
	float maxVel;

	VelocityCondition(float minVel, float maxVel);

	virtual bool IsTrue(SceneObject* so) const override;
};