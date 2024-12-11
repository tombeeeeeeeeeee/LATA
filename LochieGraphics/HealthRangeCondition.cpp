#include "HealthRangeCondition.h"

#include "SceneObject.h"
#include "Health.h"
#include "Transform.h"

#include <iostream>

HealthRangeCondition::HealthRangeCondition(int min, int max) :
	minHealth(min),
	maxHealth(max)
{
}

bool HealthRangeCondition::IsTrue(SceneObject* so) const
{
	Health* health = nullptr;
	if (so->parts & Parts::health) {
		health = so->health();
	}
	else if (so->transform()->getParent()) {
		if (so->transform()->getParent()->so->parts & Parts::health) {
			health = so->transform()->getParent()->so->health();
		}
	}
	if (health == nullptr) {
		std::cout << "SceneObject has no health part to check\n";
		return false;
	}
	return health->currHealth >= minHealth && health->currHealth < maxHealth;
}
