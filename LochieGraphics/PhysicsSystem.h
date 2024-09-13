#pragma once
#include "RigidBody.h"
#include "Transform.h"
#include <unordered_map>
#include "CollisionFunctions.h"

struct Hit;

class PhysicsSystem
{
public:
	PhysicsSystem() {};
	PhysicsSystem(toml::table table);

	void UpdateRigidBodies(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		float deltaTime
	);

	void CollisionCheckPhase
	(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		std::unordered_map<unsigned long long, Collider*>& colliders
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
	void SetCollisionLayerMaskIndexed(int layerA, int layerB, bool state);

	/// <summary>
	/// Sets the entire bit mask for a layer in physics collisions
	/// </summary>
	void SetCollisionLayerMask(int layer, unsigned int bitMask);

	bool GetCollisionLayerBool(int layerA, int layerB);
	bool GetCollisionLayerIndexed(int layerA, int layerB);
	toml::table Serialise() const;

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

	int collisionItterations = 1;

private: 
	RigidBody dummyRigidBody = RigidBody(0.0f,0.0f);

public:
	static bool RayCast(glm::vec2 pos, glm::vec2 direction, std::vector<Hit>& hits, float length = FLT_MAX, int layerMask = INT32_MAX, bool ignoreTriggers = true);

private:
	static std::unordered_map<unsigned long long, Transform>* transformsInScene;
	static std::unordered_map<unsigned long long, RigidBody>* rigidBodiesInScene;
	static std::unordered_map<unsigned long long, Collider*>* collidersInScene;

	static CollisionPacket RayCastAgainstCollider(
		glm::vec2 pos, glm::vec2 direction,
		Transform& transform, Collider* collider
		);
};