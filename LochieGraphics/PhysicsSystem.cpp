#include "PhysicsSystem.h"
#include "Hit.h"
#include "Transform.h"

std::unordered_map<unsigned long long, RigidBody>* PhysicsSystem::rigidBodiesInScene = nullptr;
std::unordered_map<unsigned long long, Transform>* PhysicsSystem::transformsInScene = nullptr;
std::unordered_map<unsigned long long, Collider*>* PhysicsSystem::collidersInScene = nullptr;

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
	transformsInScene = &transforms;
	rigidBodiesInScene = &rigidBodies;
	collidersInScene = &colliders;
	
	for (int iteratorNumber = 0; iteratorNumber < CollisionItterations; iteratorNumber++)
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
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
					a, &dummyRigidBody, transformA, transformB
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
					&dummyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
					&dummyRigidBody, a, transformB, transformA
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
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
					collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderB, (PolygonCollider*)colliderA,
					&dummyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0)
                    collisions.push_back(collision);
			}
		}
		
	}
}

void PhysicsSystem::CollisisonResolution(CollisionPacket collision)
{
	Collision collisionFromBsPerspective = { collision.rigidBodyA, collision.soA, collision.normal, collision.colliderA->collisionLayer | collision.colliderB->collisionLayer , collision.soB };
	Collision collisionFromAsPerspective = { collision.rigidBodyB, collision.soB, -collision.normal, collision.colliderA->collisionLayer | collision.colliderB->collisionLayer , collision.soA };

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

void PhysicsSystem::SetCollisionLayerMask(int a, int b, bool state)
{
	//Catch values that don't fit in the layer mask
	int layerA = glm::log2((float)a);
	int layerB = glm::log2((float)b);
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

bool PhysicsSystem::RayCast(glm::vec2 pos, glm::vec2 direction, std::vector<Hit>& hits, float length, int layerMask, bool ignoreTriggers)
{
	std::vector<CollisionPacket> collisions;
	hits.clear();
	if (glm::length(direction) == 0.0f)
	{
		return false;
	}

	for (auto& rigidBody : *rigidBodiesInScene)
	{
		for(auto& collider : rigidBody.second.colliders)
		{
			bool triggerPassing = true;
			if (collider->isTrigger && ignoreTriggers)
			{
				triggerPassing = false;
			}

			if ((collider->collisionLayer & layerMask) && triggerPassing)
			{
				CollisionPacket collision = RayCastAgainstCollider(
					pos, direction,
					(*transformsInScene)[rigidBody.first], collider
				);

				if (collision.depth >= 0.0f && collision.depth < length)
				{
					collisions.push_back(collision);
				}
			}
		}
	}

	for (auto collider = (*collidersInScene).begin(); collider != (*collidersInScene).end(); collider++)
	{
		bool triggerPassing = true;
		if (collider->second->isTrigger && ignoreTriggers)
		{
			triggerPassing = false;
		}

		if ((collider->second->collisionLayer & layerMask) && triggerPassing)
		{
			CollisionPacket collision = RayCastAgainstCollider(
				pos, direction,
				(*transformsInScene)[collider->first], collider->second
			);

			if (collision.depth >= 0.0f && collision.depth < length)
			{
				collisions.push_back(collision);
			}
		}
	}
	Hit hit;
	//Post casting ray sorting to see what has been hit.
	if (collisions.size() == 0)
	{
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
	hit.position = collisions[0].contactPoint;
	hit.distance = collisions[0].depth;
	hits.push_back(hit);

	hits.reserve(collisions.size() - 1);
	for (int i = 1; i < collisions.size(); i++)
	{
		hits.push_back(
			{
				collisions[i].normal,
				collisions[i].depth,
				collisions[i].contactPoint,
				collisions[i].colliderA,
				collisions[i].soA
			});
	}

	return hit.distance < length;
}

CollisionPacket PhysicsSystem::RayCastAgainstCollider(glm::vec2 pos, glm::vec2 direction, Transform& transform, Collider* collider)
{
	// COLLISION BODY A is the target COLLISION BODY B SHOULD BE NULL
	CollisionPacket collision;
	collision.depth = -1.0f;

	if (collider->getType() == ColliderType::plane)
	{
		PlaneCollider* plane = (PlaneCollider*)collider;
		float bDot = glm::dot(plane->normal, direction);
		if(bDot > 0.0f)
		{
			float t = plane->displacement - glm::dot(pos, plane->normal);
			t /= bDot;
			if (t > 0.0f)
			{
				collision.contactPoint = pos + direction * t;
				collision.colliderA = plane;
				collision.normal = plane->normal;
				collision.depth = glm::length(direction * t);
				collision.soA = transform.getSceneObject();
			}
		}
	}
	else if(collider->getType() == ColliderType::polygon)
	{
		PolygonCollider* poly = ((PolygonCollider*)collider);
		if(poly->verts.size() == 1)
		{
			//MAKE RAYCAST AGAINST CIRCLE
			glm::vec2 centre = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[0]);
			glm::vec2 f = pos - centre;
			float a = direction.x * direction.x + direction.y * direction.y;
			float b = f.x * direction.x + f.y * direction.y;
			b *= 2.0f;
			float c = f.x * f.x + f.y * f.y - poly->radius * poly->radius;
			float t;
			float discriminant = b * b - 4.0f * a * c;
			if (discriminant == 0.0f)
			{
				t = -b / 2.0f * a;
			}
			else if (discriminant > 0.0f)
			{
				float t1 = -b + sqrt(discriminant);
				t1 /= 2.0f * a;
				float t2 = -b - sqrt(discriminant);
				t2 /= 2.0f * a;

				t = fminf(t1, t2);
			}
			else return collision;

			if (t > 0.0f)
			{
				collision.contactPoint = pos + direction * t;
				collision.colliderA = poly;
				collision.normal = glm::normalize(collision.contactPoint - centre);
				collision.depth = glm::length(direction * t);
				collision.soA = transform.getSceneObject();
			}
		}
		else
		{
			for (int i = 0; i < poly->verts.size(); i++)
			{
				glm::vec2 c = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[i]);
				glm::vec2 d = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[(i + 1) % poly->verts.size()]) - c;
				//glm::vec2 normal = { -d.y, d.x };
				glm::vec2 normal = { d.y, -d.x };

				float denominator = glm::dot(normal, direction);
				if (denominator <= 0) continue;
				float numerator = pos.y * direction.x - direction.y * pos.x - c.y * direction.x + direction.y * c.x;
				float t2 = numerator / denominator;
				if (t2 >= 0 && t2 <= 1)
				{
					float t1;
					if (abs(direction.x) > abs(direction.y))
					{
						t1 = c.x + d.x * t2 - pos.x;
						t1 /= direction.x;
					}
					else
					{
						t1 = c.y + d.y * t2 - pos.y;
						t1 /= direction.y;
					}

					if (t1 > 0.0f)
					{
						collision.contactPoint = pos + direction * t1;
						collision.colliderA = poly;
						collision.normal = normal;
						collision.depth = glm::length(direction * t1);
						collision.soA = transform.getSceneObject();
					}
				}
			}
		}
	}
	return collision;
}
