#pragma once
#include "Condition.h"

class SceneObject;

class HealthRangeCondition : public Condition
{
public:
	int minHealth;
	int maxHealth;

	// Min is inclusive, max is exclusive
	HealthRangeCondition(int min, int max);

	virtual bool IsTrue(SceneObject* so) const;
};
