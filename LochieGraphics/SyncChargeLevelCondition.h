#pragma once
#include "Condition.h"

#include "SyncChargeLevel.h"

class SyncChargeLevelCondition : public Condition
{
public:

	bool charging = false;
	ChargeLevel level;
	SyncChargeLevelCondition(ChargeLevel _level, bool _charging);

	virtual bool IsTrue(SceneObject* so) const override;
};

