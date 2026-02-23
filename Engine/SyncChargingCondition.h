#pragma once
#include "Condition.h"

class SyncChargingCondition : public Condition
{
public:
	bool flipped;

	SyncChargingCondition(bool _flipped = false);

	virtual bool IsTrue(SceneObject* so) const override;
};

