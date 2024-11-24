#include "SyncChargingCondition.h"

#include "SceneObject.h"
#include "Sync.h"
#include "Transform.h"

SyncChargingCondition::SyncChargingCondition(bool _flipped) :
    flipped(_flipped)
{
}

bool SyncChargingCondition::IsTrue(SceneObject* so) const
{
    Sync* sync = so->sync();
    if (!sync) {
        // TODO: Just set the sync component to check
        if (so->transform()->getParent()) {
            sync = so->transform()->getParent()->so->sync();
        }

        if (!sync) {
            // TODO: Warning for no sync part
            return false;
        }
    }
    return sync->chargingShot && !flipped;
}
