#include "SyncChargeLevelCondition.h"

#include "SceneObject.h"
#include "Sync.h"
#include "Transform.h"

SyncChargeLevelCondition::SyncChargeLevelCondition(ChargeLevel _level, bool _charging) :
	level(_level),
	charging(_charging)
{
}

bool SyncChargeLevelCondition::IsTrue(SceneObject* so) const
{
	Sync* sync = so->sync();
	if (!sync) {
		// TODO: Just set the sync to check
		if (so->transform()->getParent()) {
			sync = so->transform()->getParent()->so->sync();
		}
		if (!sync) {
			return false;
		}
	}
	return (sync->lastShotLevel == level && charging == sync->chargingShot);
}
