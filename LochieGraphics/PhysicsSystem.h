#pragma once
#include "RigidBody.h"
#include <unordered_map>
#include "CollisionFunctions.h"

struct Hit;
class Transform;

const int CollisionItterations = 10;

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
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		std::unordered_map<unsigned long long, Collider*> & colliders
	);

	void GetCollisions(
		RigidBody* a, RigidBody* b,
		Transform* transformA, Transform* transformB,
		std::vector<CollisionPacket>& collisions
	);

	void GetCollisions(
		RigidBody* a, Collider* b,
		Transform* transformA, Transform* transformB,
		std::vector<CollisionPacket> collisions
	);

	void CollisisonResolution(CollisionPacket collision);

	/// <summary>
	/// Set the collision layer mask to true or false for two collision layers
	/// </summary>
	void SetCollisionLayerMask(int layerA, int layerB, bool state);

	/// <summary>
	/// Sets the entire bit mask for a layer in physics collisions
	/// </summary>
	void SetCollisionLayerMask(int layer, unsigned int bitMask);

	bool GetCollisionLayerBool(int layerA, int layerB);

	unsigned int layerMasks[32] = {
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
		UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX,
	};

private: 
	RigidBody dumbyRigidBody = RigidBody(0.0f,0.0f);

public:
	static bool RayCast(glm::vec2 pos, glm::vec2 direction, Hit& hit, float length = FLT_MAX, int layerMask = INT32_MAX, bool ignoreTriggers = true);

private:
	static std::unordered_map<unsigned long long, Transform>& transformsInScene;
	static std::unordered_map<unsigned long long, RigidBody>& rigidiBodiesInScene;
	static std::unordered_map<unsigned long long, Collider*>& collidersInScene;

	static CollisionPacket RayCastAgainstCollider(
		glm::vec2 pos, glm::vec2 direction,
		Transform& transform, Collider* collider
		);
};