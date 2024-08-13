#pragma once
class RigidBody;
struct Collider;

struct CollisionPacket
{
	/// <summary>
	/// Normal points from object a to object b;
	/// </summary>
	glm::vec2 normal = {0,0};
	float depth = NAN;
	glm::vec2 contactPoint = { 0,0 };

	RigidBody* rigidBodyA = nullptr;
	RigidBody* rigidBodyB = nullptr;

	Collider* colliderA = nullptr;
	Collider* colliderB = nullptr;

	SceneObject* soA = 0;
	SceneObject* soB = 0;

	glm::vec2 tangentA;
	glm::vec2 tangentB;
};