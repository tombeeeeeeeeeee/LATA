#include "PhysicsSystem.h"
#include "Hit.h"
#include "Transform.h"

void PhysicsSystem::UpdateRigidBodies(
	std::unordered_map<unsigned long long, Transform>& transforms,
	std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
	float deltaTime
)
{
	for (auto i = rigidBodies.begin(); i != rigidBodies.end(); i++)
	{
		if (i->second.isStatic) continue;

		RigidBody* rb = &i->second;

		glm::vec2 pos = rb->netDepen;
		rb->accel = rb->netForce * rb->invMass;
		rb->vel =  rb->vel + rb->accel * deltaTime;
		pos += rb->vel * deltaTime;

		rb->netDepen = { 0,0 };
		rb->netForce = { 0,0 };

		glm::vec3 eulers = transforms[i->first].getEulerRotation();

		transforms[i->first].setPosition(transforms[i->first].getPosition() + glm::vec3(pos.x, 0.0f, pos.y));
		transforms[i->first].setRotation(
			transforms[i->first].getRotation()
			*
			glm::rotate(glm::identity<glm::quat>(), rb->angularVel * deltaTime, {0.0f,1.0f,0.0f})
		);
	}
}

void PhysicsSystem::CollisionCheckPhase(
	std::unordered_map<unsigned long long, Transform>& transforms,
	std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
	std::unordered_map<unsigned long long, Collider*>& colliders

)
{
	transformsInScene = transforms;
	rigidiBodiesInScene = rigidBodies;
	collidersInScene = colliders;


	for (int i = 0; i < CollisionItterations; i++)
	{
		std::vector<CollisionPacket> collisions;
		for (auto i = rigidBodies.begin(); i != rigidBodies.end(); i++)
		{
			for (auto j = std::next(i); j != rigidBodies.end(); j++)
			{
				if (i->second.isStatic && j->second.isStatic) continue;
				GetCollisions(
					&i->second, &j->second,
					&transforms[i->first], &transforms[j->first],
					collisions);
			}

			if (!i->second.isStatic)
			{
				for (auto k = colliders.begin(); k != colliders.end(); k++)
				{
					
					GetCollisions(
						&i->second, k->second,
						&transforms[i->first], &transforms[k->first],
						collisions);
				}
			}
		}

		for (int i = 0; i < collisions.size(); i++)
		{
			CollisisonResolution(collisions[i]);
		}
	}
}

void PhysicsSystem::GetCollisions(
	RigidBody* a, RigidBody* b,
	Transform* transformA, Transform* transformB,
	std::vector<CollisionPacket>& collisions)
{


	std::vector<Collider*> aCols = (*a->getColliders());
	std::vector<Collider*> bCols = (*b->getColliders());

	for (Collider* colliderA : aCols)
	{
		for (Collider* colliderB : bCols)
		{
			if (!GetCollisionLayerBool(colliderA->collisionLayer, colliderB->collisionLayer))
				continue;

			if (colliderA->getType() == ColliderType::polygon && ((PolygonCollider*)colliderA)->verts.size() == 1)
			{
				if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnCircleCollision(
						(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::plane)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
						(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::polygon)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
						(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
			}
			else if (colliderA->getType() == ColliderType::plane)
			{
				if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
						(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
						b, a, transformB, transformA
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::polygon)
				{
					CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
						(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
						b, a, transformB, transformA
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
			}
			else if (colliderA->getType() == ColliderType::polygon)
			{
				if (colliderB->getType() == ColliderType::polygon)
				{
					CollisionPacket collision = CollisionFunctions::PolyOnPolyCollision(
						(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::plane)
				{
					CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
						(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
						(PolygonCollider*)colliderB, (PolygonCollider*)colliderA,
						b, a, transformB, transformA
					);
					if (collision.depth >= 0) collisions.push_back(collision);
				}
			}
		}
	}
}

void PhysicsSystem::GetCollisions(RigidBody* a, Collider* b, Transform* transformA, Transform* transformB, std::vector<CollisionPacket> collisions)
{
	std::vector<Collider*> aCols = (*a->getColliders());
	Collider* colliderB = b;

	for (Collider* colliderA : aCols)
	{
		if (!GetCollisionLayerBool(colliderA->collisionLayer, colliderB->collisionLayer))
			continue;

		if (colliderA->getType() == ColliderType::polygon && ((PolygonCollider*)colliderA)->verts.size() == 1)
		{
			if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnCircleCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
		}
		else if (colliderA->getType() == ColliderType::plane)
		{
			if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
					(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
					&dumbyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
					&dumbyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
		}
		else if (colliderA->getType() == ColliderType::polygon)
		{
			if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPolyCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
					collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderB, (PolygonCollider*)colliderA,
					&dumbyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
		}
		
	}
}

void PhysicsSystem::CollisisonResolution(CollisionPacket collision)
{
	Collision collisionFromBsPerspective = { collision.rigidBodyA, collision.soA, collision.normal, collision.colliderA->collisionLayer | collision.colliderB->collisionLayer};
	Collision collisionFromAsPerspective = { collision.rigidBodyB, collision.soB, -collision.normal, collision.colliderA->collisionLayer | collision.colliderB->collisionLayer};

	if (collision.depth < 0) return;
	if (collision.colliderA->isTrigger && collision.colliderB->isTrigger) return;
	else if (collision.colliderA->isTrigger)
	{
		for (int i = 0; i < collision.rigidBodyA->onTrigger.size(); i++) 
		{
			collision.rigidBodyA->onTrigger[i](collisionFromAsPerspective);
		}
	}
	else if (collision.colliderB->isTrigger)
	{
		for (int i = 0; i < collision.rigidBodyB->onTrigger.size(); i++)
		{
			collision.rigidBodyB->onTrigger[i](collisionFromBsPerspective);
		}
	}

	for (int i = 0; i < collision.rigidBodyA->onCollision.size(); i++)
	{
		collision.rigidBodyA->onCollision[i](collisionFromAsPerspective);
	}
	for (int i = 0; i < collision.rigidBodyB->onCollision.size(); i++)
	{
		collision.rigidBodyB->onCollision[i](collisionFromBsPerspective);
	}


	if (collision.rigidBodyA->invMass + collision.rigidBodyB->invMass == 0) return;

	glm::vec2 radiusPerpA = collision.tangentA;
	glm::vec2 radiusPerpB = collision.tangentB;

	float totalMass = collision.rigidBodyA->invMass + collision.rigidBodyB->invMass;

	collision.rigidBodyA->AddDepen(collision.normal * collision.depth * collision.rigidBodyA->invMass / totalMass);
	collision.rigidBodyB->AddDepen(-collision.normal * collision.depth * collision.rigidBodyB->invMass / totalMass);

	glm::vec2 relativeVelocity = 
		collision.rigidBodyA->vel - collision.rigidBodyB->vel;
		//(collision.rigidBodyA->vel + collision.rigidBodyA->angularVel * radiusPerpA)
		//- (collision.rigidBodyB->vel + collision.rigidBodyB->angularVel * radiusPerpB);

	float totalInverseMass = (collision.rigidBodyA->invMass + collision.rigidBodyB->invMass);

	float j = -(1.0f + glm::max(collision.rigidBodyA->elasticicty, collision.rigidBodyB->elasticicty)) * glm::dot(relativeVelocity, collision.normal) /
		(totalInverseMass);// +(float)pow(glm::dot(radiusPerpA, collision.normal), 2) * collision.rigidBodyA->invMomentOfInertia
			//+ (float)pow(glm::dot(radiusPerpB, collision.normal), 2) * collision.rigidBodyB->invMomentOfInertia);

	if (j <= 0.0f) return;

	glm::vec2 linearRestitution = j * collision.normal;

	collision.rigidBodyA->AddImpulse(linearRestitution);
	collision.rigidBodyB->AddImpulse(-linearRestitution);

	//if (abs(glm::dot(radiusPerpA, collision.normal)) > 0.000001f)
	//	collision.rigidBodyA->AddRotationalImpulse(glm::dot(radiusPerpA, linearRestitution));
	//
	//if (abs(glm::dot(radiusPerpB, collision.normal)) > 0.000001f)
	//	collision.rigidBodyB->AddRotationalImpulse(glm::dot(radiusPerpB, -linearRestitution));

}

void PhysicsSystem::SetCollisionLayerMask(int layerA, int layerB, bool state)
{
	//Catch values that don't fit in the layer mask
	if (layerA < 0 || layerA >= 32) return;
	if (layerB < 0 || layerB >= 32) return;


	layerMasks[layerA] &= ~(1 << layerB);
	layerMasks[layerB] &= ~(1 << layerA);


	if (state)
	{
		layerMasks[layerA] |= (1 << layerB);
		layerMasks[layerB] |= (1 << layerA);
	}
}

void PhysicsSystem::SetCollisionLayerMask(int layer, unsigned int bitMask)
{
	layerMasks[layer] = bitMask;
	for (unsigned int i = 0; i < 32; i++)
	{
		layerMasks[i] &= ~(1 << layer);
		int on = (bitMask & (1 << i)) == (1 << i) ? 1 : 0;
		layerMasks[i] |= (on << layer);
	}
}

bool PhysicsSystem::GetCollisionLayerBool(int layerA, int layerB)
{
	return layerMasks[layerA] & (1 << layerB);
}

bool PhysicsSystem::RayCast(glm::vec2 pos, glm::vec2 direction, Hit& hit, float length, int layerMask, bool ignoreTriggers)
{
	std::vector<CollisionPacket> collisions;


	for (auto rigidBody = rigidiBodiesInScene.begin(); rigidBody != rigidiBodiesInScene.end(); rigidBody++)
	{
		for (auto collider = rigidBody->second.colliders.begin(); collider != rigidBody->second.colliders.end(); collider++)
		{
			CollisionPacket collision = RayCastAgainstCollider(
				pos, direction,
				transformsInScene[rigidBody->first], *collider
			);
			if (collision.depth >= 0.0f && collision.depth < length)
			{
				collisions.push_back(collision);
			}
		}
	}

	for (auto collider = collidersInScene.begin(); collider != collidersInScene.end(); collider++)
	{
		CollisionPacket collision = RayCastAgainstCollider(
			pos, direction,
			transformsInScene[collider->first], collider->second
		);

		if (collision.depth >= 0.0f && collision.depth < length)
		{
			collisions.push_back(collision);
		}
	}

	//Post casting ray sorting to see what has been hit.
	if (collisions.size() > 0)
	{
		hit.collider = nullptr;
		hit.sceneObject = nullptr;
		hit.normal = { 0.0f, 0.0f };
		hit.distance = NAN;
		hit.otherCollisions = {};
		return false;
	}

	//bubble sort for smallest distance from ray
	for(int i = 0; i < collisions.size(); i++)
	{
		for (int j = i + 1; j < collisions.size(); j++)
		{
			if (collisions[i].depth > collisions[j].depth)
			{
				CollisionPacket temp = collisions[i];
				collisions[i] = collisions[j];
				collisions[j] = temp;
			}
		}
	}

	hit.sceneObject = collisions[0].soA;
	hit.collider = collisions[0].colliderA;
	hit.normal = collisions[0].normal;
	hit.distance = collisions[0].depth;
	if (collisions.size() == 1)
	{
		hit.otherCollisions = {};
	}
	else
	{
		hit.otherCollisions.reserve(collisions.size() - 1);
		for (int i = 1; i < collisions.size(); i++)
		{
			hit.otherCollisions.push_back(collisions[i].soA);
		}
	}

	return hit.distance < length;
}

CollisionPacket PhysicsSystem::RayCastAgainstCollider(glm::vec2 pos, glm::vec2 direction, Transform& transform, Collider* collider)
{
	// COLLISION BODY A is the target COLLISION BODY B SHOULD BE NULL

	return CollisionPacket();
	/*
		TODO:
		MAKE RAYCAST AGAINST CIRCLE
		MAKE RAYCAST AGAINST PLANE
		MAKE RAYCAST AGAINST POLY

	*/
}
