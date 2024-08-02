#pragma once
#include "RigidBody.h"
#include "Transform.h"
#include "CollisionPacket.h"

namespace CollisionFunctions
{
	static CollisionPacket CircleOnCircleCollision(
		PolygonCollider* circleA, PolygonCollider* circleB, 
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	static CollisionPacket CircleOnPolyCollision(
		PolygonCollider* circle, PolygonCollider* poly,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	//TODO
	static CollisionPacket CircleOnDirectionalPolyCollision(
		PolygonCollider* circle, DirectionalCollider directionalPoly,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	static CollisionPacket CircleOnPlaneCollision(
		PolygonCollider* circle, PlaneCollider* plane,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	static CollisionPacket PolyOnPolyCollision(
		PolygonCollider* polyA, PolygonCollider* polyB,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	//TODO:
	static CollisionPacket PolyOnDirectionalPolyCollision(
		PolygonCollider* poly, DirectionalCollider* directionalPoly,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	static CollisionPacket PolyOnPlaneCollision(
		PolygonCollider* poly, PlaneCollider* plane,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);

	//TODO: 
	static CollisionPacket DirectionalPolyOnPlaneCollision(
		DirectionalCollider* directionalPoly, PlaneCollider* plane,
		RigidBody* rigidBodyA, RigidBody* rigidBodyB,
		Transform* transformA, Transform* transformB
	);
};