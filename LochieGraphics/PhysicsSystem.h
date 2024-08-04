#pragma once
#include "RigidBody.h"
#include "Transform.h"
#include <unordered_map>
#include "CollisionFunctions.h"

class PhysicsSystem
{
public:
	void UpdateRigidBodies(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		float deltaTime
	);

	void CollisionCheckPhase
	(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies
	);

	void GetCollisions(
		RigidBody* a, RigidBody* b,
		Transform* transformA, Transform* transformB,
		std::vector<CollisionPacket> collisions
	);

	void CollisisonResolution(CollisionPacket collision);
};