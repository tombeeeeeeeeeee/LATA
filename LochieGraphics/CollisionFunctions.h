#pragma once
#include "CollisionPacket.h"
#include <vector>
#include "Maths.h"

class RigidBody;
class Transform;
struct PolygonCollider;
struct DirectionalCollider;
struct PlaneCollider;

namespace CollisionFunctions
{
	CollisionPacket CircleOnCircleCollision(
		PolygonCollider* circleA, PolygonCollider* circleB, 
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	CollisionPacket CircleOnPolyCollision(
		PolygonCollider* circle, PolygonCollider* poly,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	bool CircleOnPolyCheck(
		glm::vec2 circlePos, float radius,
		PolygonCollider* poly, Transform transformB
	);

	//TODO
	CollisionPacket CircleOnDirectionalPolyCollision(
		PolygonCollider* circle, DirectionalCollider directionalPoly,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	CollisionPacket CircleOnPlaneCollision(
		PolygonCollider* circle, PlaneCollider* plane,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	CollisionPacket PolyOnPolyCollision(
		PolygonCollider* polyA, PolygonCollider* polyB,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	//TODO:
	CollisionPacket PolyOnDirectionalPolyCollision(
		PolygonCollider* poly, DirectionalCollider* directionalPoly,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	CollisionPacket PolyOnPlaneCollision(
		PolygonCollider* poly, PlaneCollider* plane,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	//TODO: 
	CollisionPacket DirectionalPolyOnPlaneCollision(
		DirectionalCollider* directionalPoly, PlaneCollider* plane,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	glm::vec2 SATMinMax(glm::vec2 axis, std::vector<glm::vec2> verts);
};