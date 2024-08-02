#pragma once
#include "RigidBody.h"
#include "Transform.h"
#include "CollisionPacket.h"

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
};