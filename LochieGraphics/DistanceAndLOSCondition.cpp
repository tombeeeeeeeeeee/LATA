#include "DistanceAndLOSCondition.h"
#include "SceneObject.h"
#include "Enemy.h"
#include "Transform.h"

bool DistanceAndLOSCondition::IsTrue(SceneObject* so)
{
	Enemy* enemy = so->enemy();
	if (enemy)
	{
		if (enemy->hasLOS)
		{
			if (IsTrueWhenLessThan)
			{
				return glm::length(so->transform()->getGlobalPosition() - enemy->lastTargetPos) < range;
			}
			else
			{
				return glm::length(so->transform()->getGlobalPosition() - enemy->lastTargetPos) > range;
			}
		}
	}
	return false;
}
