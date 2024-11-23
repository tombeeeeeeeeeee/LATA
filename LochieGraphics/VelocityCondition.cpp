#include "VelocityCondition.h"

#include "SceneObject.h"
#include "RigidBody.h"
#include "Transform.h"

#include <iostream>

VelocityCondition::VelocityCondition(float _minVel, float _maxVel) :
	minVel(_minVel),
	maxVel(_maxVel)
{
}

bool VelocityCondition::IsTrue(SceneObject* so) const
{
	// TODO: assign the rigidbody to check
	RigidBody* rb = so->rigidbody();
	if (!rb) {
		if (so->transform()->getParent()) {
			rb = so->transform()->getParent()->so->rigidbody();
		}
		if (!rb) {
			std::cout << "No rigidbody found for velocity\n";
			return false;
		}
	}
	float mag = glm::length(rb->vel);
	return (mag > minVel) && (mag < maxVel);
}
