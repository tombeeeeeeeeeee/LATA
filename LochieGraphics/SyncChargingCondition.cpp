#include "SyncChargingCondition.h"

#include "SceneObject.h"
#include "Sync.h"

SyncChargingCondition::SyncChargingCondition(bool _flipped) :
    flipped(_flipped)
{
}

bool SyncChargingCondition::IsTrue(SceneObject* so) const
{
    Sync* sync = so->sync();
    if (!sync) {
        // TODO: Warning for no sync part
        return false;
    }
    return sync->chargingShot && !flipped;
}
