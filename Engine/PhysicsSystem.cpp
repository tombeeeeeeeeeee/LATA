#include "PhysicsSystem.h"

#include "Hit.h"
#include "Transform.h"
#include "Collider.h"
#include "Collision.h"
#include "CollisionPacket.h"
#include "CollisionFunctions.h"

#include "Serialisation.h"

#include <algorithm>

std::unordered_map<unsigned long long, RigidBody>* PhysicsSystem::rigidBodiesInScene = nullptr;
std::unordered_map<unsigned long long, Transform>* PhysicsSystem::transformsInScene = nullptr;
std::unordered_map<unsigned long long, Collider*>* PhysicsSystem::collidersInScene = nullptr;

PhysicsSystem::PhysicsSystem(toml::table table)
{
	collisionItterations = Serialisation::LoadAsInt(table["collisionItterations"]);

	for (int i = 0; i < 32; i++)
	{
		layerMasks[i] = Serialisation::LoadAsInt(table["layerMask" + i]);
	}
}

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

		glm::vec2 pos = {0.0f, 0.0f};
		rb->accel = rb->netForce * rb->invMass;
		rb->vel =  rb->vel + rb->accel * deltaTime;
		pos += rb->vel * deltaTime;

		rb->netDepen = { 0,0 };
		rb->netForce = { 0,0 };

		Transform& t = transforms.at(i->first);
		glm::vec3 eulers = t.getEulerRotation();

		t.setPosition(t.getPosition() + glm::vec3(pos.x, 0.0f, pos.y));
		t.setRotation(
			t.getRotation()
			*
			glm::rotate(glm::identity<glm::quat>(), rb->angularVel * deltaTime, {0.0f,1.0f,0.0f})
		);
	}
}

void PhysicsSystem::DepenertrationStep(std::unordered_map<unsigned long long, Transform>& transforms, std::unordered_map<unsigned long long, RigidBody>& rigidBodies)
{
	for (auto& i : rigidBodies)
	{
		if (i.second.isStatic) continue;
		transforms[i.first].setPosition(transforms[i.first].getPosition() + glm::vec3(i.second.netDepen.x, 0.0f, i.second.netDepen.y));
		i.second.netDepen = { 0.0f, 0.0f };
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

	std::vector<CollisionPacket> collisions;
	for (auto i = rigidBodies.begin(); i != rigidBodies.end(); i++)
	{
		if ((*i).second.isStatic) continue;
		for (auto j = rigidBodies.begin(); j != rigidBodies.end(); j++)
		{
			if (i->first == j->first) continue;
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

	for (int iteratorNumber = 0; iteratorNumber < collisionItterations; iteratorNumber++)
	{
		for (auto& i : collisions)
		{
			CollisisonResolution(i);
		}
	}
	DepenertrationStep(transforms, rigidBodies);



	if (displayAllColliders)
	{
		for (auto i = rigidBodies.begin(); i != rigidBodies.end(); i++)
		{
			i->second.DebugDraw(&transforms[i->first]);
		}
		for (auto k = colliders.begin(); k != colliders.end(); k++)
		{
			k->second->DebugDraw(&transforms[k->first]);
		}
	}
}

void PhysicsSystem::GetCollisions(
	RigidBody* a, RigidBody* b,
	Transform* transformA, Transform* transformB,
	std::vector<CollisionPacket>& collisions)
{
	float collidableDistance = a->maxVertDistance + b->maxVertDistance;
	collidableDistance += collisionBuffer;
	collidableDistance *= collidableDistance;
	if (glm::dot(transformA->get2DGlobalPosition() - transformB->get2DGlobalPosition(), transformA->get2DGlobalPosition() - transformB->get2DGlobalPosition()) > collidableDistance ) return;
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
					if (collision.depth >= -10) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::plane)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
						(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= -10) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::polygon)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
						(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= -10) collisions.push_back(collision);
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
					if (collision.depth >= -10) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::polygon)
				{
					CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
						(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
						b, a, transformB, transformA
					);
					if (collision.depth >= -10) collisions.push_back(collision);
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
					if (collision.depth >= -10) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::plane)
				{
					CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
						(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
						a, b, transformA, transformB
					);
					if (collision.depth >= -10) collisions.push_back(collision);
				}
				else if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
				{
					CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
						(PolygonCollider*)colliderB, (PolygonCollider*)colliderA,
						b, a, transformB, transformA
					);
					if (collision.depth >= -10) collisions.push_back(collision);
				}
			}
		}
	}
}

void PhysicsSystem::GetCollisions(RigidBody* a, Collider* b, Transform* transformA, Transform* transformB, std::vector<CollisionPacket>& collisions)
{
	std::vector<Collider*> aCols = (*a->getColliders());
	Collider* colliderB = b;
	if (glm::dot(transformA->get2DGlobalPosition() - transformB->get2DGlobalPosition(), transformA->get2DGlobalPosition() - transformB->get2DGlobalPosition()) > (minCollisonDistance + collisionBuffer) * (collisionBuffer + minCollisonDistance)) return;
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
				if (collision.depth >= -collisionBuffer)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= -collisionBuffer)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)colliderB,
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= -collisionBuffer)
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
				if (collision.depth >= -10)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderB, (PlaneCollider*)colliderA,
					&dummyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= -10)
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
				if (collision.depth >= -10)
					collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)colliderB,
					a, &dummyRigidBody, transformA, transformB
				);
				if (collision.depth >= -10)
                    collisions.push_back(collision);
			}
			else if (colliderB->getType() == ColliderType::polygon && ((PolygonCollider*)colliderB)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderB, (PolygonCollider*)colliderA,
					&dummyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= -10)
                    collisions.push_back(collision);
			}
		}
		
	}
}

void PhysicsSystem::CollisisonResolution(CollisionPacket collision)
{

	Collision collisionFromBsPerspective = { collision.rigidBodyA, collision.soA, collision.normal, collision.colliderA->collisionLayer | collision.colliderB->collisionLayer, collision.soB };
	Collision collisionFromAsPerspective = { collision.rigidBodyB, collision.soB, -collision.normal, collision.colliderA->collisionLayer | collision.colliderB->collisionLayer, collision.soA };

	if (collision.depth < 0)
	{
		if (collision.colliderA->isTrigger || collision.colliderB->isTrigger) return;
		float totalMass = collision.rigidBodyA->invMass + collision.rigidBodyB->invMass;

		collision.rigidBodyA->AddDepen(collision.normal, collision.depth * collision.rigidBodyA->invMass / totalMass);
		collision.rigidBodyB->AddDepen(-collision.normal, collision.depth * collision.rigidBodyB->invMass / totalMass);
		return;
	}

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

	if (collision.colliderA->isTrigger || collision.colliderB->isTrigger) return;
	else if (collision.rigidBodyA->invMass + collision.rigidBodyB->invMass == 0) return;

	float totalMass = collision.rigidBodyA->invMass + collision.rigidBodyB->invMass;

	collision.rigidBodyA->AddDepen(collision.normal, collision.depth * collision.rigidBodyA->invMass / totalMass);
	collision.rigidBodyB->AddDepen(-collision.normal, collision.depth * collision.rigidBodyB->invMass / totalMass);

	glm::vec2 radiusPerpA = collision.tangentA;
	glm::vec2 radiusPerpB = collision.tangentB;

	glm::vec2 relativeVelocity = collision.rigidBodyA->vel - collision.rigidBodyB->vel;

	float totalInverseMass = (collision.rigidBodyA->invMass + collision.rigidBodyB->invMass);

	float j = -(1.0f + glm::max(glm::max(collision.rigidBodyA->elasticicty, collision.rigidBodyB->elasticicty), 0.2f))
		* glm::dot(relativeVelocity, collision.normal) / (totalInverseMass);// +(float)pow(glm::dot(radiusPerpA, collision.normal), 2) * collision.rigidBodyA->invMomentOfInertia
			//+ (float)pow(glm::dot(radiusPerpB, collision.normal), 2) * collision.rigidBodyB->invMomentOfInertia);

	if (j <= 0.0f) return;

	glm::vec2 linearRestitution = j * collision.normal;

	if(!collision.rigidBodyA->ignoreThisCollision)
		collision.rigidBodyA->AddImpulse(linearRestitution);
	if (!collision.rigidBodyB->ignoreThisCollision)
		collision.rigidBodyB->AddImpulse(-linearRestitution);

	//if (abs(glm::dot(radiusPerpA, collision.normal)) > 0.000001f)
	//	collision.rigidBodyA->AddRotationalImpulse(glm::dot(radiusPerpA, linearRestitution));
	//
	//if (abs(glm::dot(radiusPerpB, collision.normal)) > 0.000001f)
	//	collision.rigidBodyB->AddRotationalImpulse(glm::dot(radiusPerpB, -linearRestitution));
	collision.rigidBodyA->ignoreThisCollision = false;
	collision.rigidBodyB->ignoreThisCollision = false;
}

void PhysicsSystem::SetCollisionLayerMask(int a, int b, bool state)
{
	//Catch values that don't fit in the layer mask
	int layerA = (int)glm::log2((float)a);
	int layerB = (int)glm::log2((float)b);
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

void PhysicsSystem::SetCollisionLayerMaskIndexed(int layerA, int layerB, bool state)
{
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
	int layerIndex = (int)log2((float)layer);
	layerMasks[layerIndex] = bitMask;
	for (unsigned int i = 0; i < 32; i++)
	{
		layerMasks[i] &= ~layer;
		int on = (bitMask & (1 << i)) == (1 << i) ? 1 : 0;
		layerMasks[i] |= (on << layerIndex);
	}
}

bool PhysicsSystem::GetCollisionLayerBool(const int a, const int b)
{
	int layerA = (int)log2((float)a);
	int layerB = (int)log2((float)b);
	return layerMasks[layerA] & (1 << layerB);
}

bool PhysicsSystem::GetCollisionLayerIndexed(const int layerA, const int layerB)
{
	return layerMasks[layerA] & (1 << layerB);
}

toml::table PhysicsSystem::Serialise() const
{
	return toml::table{
		{ "collisionItterations", collisionItterations },
		{ "layerMask0", layerMasks[0] },
		{ "layerMask1", layerMasks[1] },
		{ "layerMask2", layerMasks[2] },
		{ "layerMask3", layerMasks[3] },
		{ "layerMask4", layerMasks[4] },
		{ "layerMask5", layerMasks[5] },
		{ "layerMask6", layerMasks[6] },
		{ "layerMask7", layerMasks[7] },
		{ "layerMask8", layerMasks[8] },
		{ "layerMask9", layerMasks[9] },
		{ "layerMask10", layerMasks[10] },
		{ "layerMask11", layerMasks[11] },
		{ "layerMask12", layerMasks[12] },
		{ "layerMask13", layerMasks[13] },
		{ "layerMask14", layerMasks[14] },
		{ "layerMask15", layerMasks[15] },
		{ "layerMask16", layerMasks[16] },
		{ "layerMask17", layerMasks[17] },
		{ "layerMask18", layerMasks[18] },
		{ "layerMask19", layerMasks[19] },
		{ "layerMask20", layerMasks[20] },
		{ "layerMask21", layerMasks[21] },
		{ "layerMask22", layerMasks[22] },
		{ "layerMask23", layerMasks[23] },
		{ "layerMask24", layerMasks[24] },
		{ "layerMask25", layerMasks[25] },
		{ "layerMask26", layerMasks[26] },
		{ "layerMask27", layerMasks[27] },
		{ "layerMask28", layerMasks[28] },
		{ "layerMask29", layerMasks[29] },
		{ "layerMask30", layerMasks[30] },
		{ "layerMask31", layerMasks[31] },
	};
}


/// <param name="pos">Starting position of cast</param>
/// <param name="direction">direction of the raycast</param>
/// <param name="hits"> output of hits</param>
/// <param name="length"> distance raycast can travel</param>
/// <param name="layerMask"> layers that the raycast will hit</param>
/// <param name="ignoreTriggers"> whether the raycast ignores triggers, is true by default</param>
/// <returns> whether it hit anything</returns>
bool PhysicsSystem::RayCast(glm::vec2 pos, glm::vec2 direction, std::vector<Hit>& hits, float length, int layerMask, bool ignoreTriggers)
{
	return RayCastRadiusExpansion(pos, direction, hits, 0.0f, length, layerMask, ignoreTriggers);
}

bool PhysicsSystem::RayCastRadiusExpansion(glm::vec2 pos, glm::vec2 direction, std::vector<Hit>& hits,float radiusExpansion, float length, int layerMask, bool ignoreTriggers)
{
	std::vector<CollisionPacket> collisions;
	hits.clear();
	if (glm::length(direction) == 0.0f || rigidBodiesInScene == nullptr)
	{
		return false;
	}

	for (auto& rigidBody : *rigidBodiesInScene)
	{
		for(auto& collider : *rigidBody.second.getColliders())
		{
			bool triggerPassing = true;
			if (collider->isTrigger && ignoreTriggers)
			{
				triggerPassing = false;
			}

			if (triggerPassing && (collider->collisionLayer & layerMask))
			{
				CollisionPacket collision = RayCastAgainstCollider(
					pos, direction,
					(*transformsInScene)[rigidBody.first], collider,
					radiusExpansion
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
				(*transformsInScene)[collider->first], collider->second,
				radiusExpansion
			);

			if (collision.depth >= 0.0f && collision.depth < length)
			{
				collisions.push_back(collision);
			}
		}
	}

	//Post casting ray sorting to see what has been hit.
	if (collisions.size() == 0)
	{
		return false;
	}

	std::sort(collisions.begin(), collisions.end(), [](const CollisionPacket& a, const CollisionPacket& b) {
		return a.depth < b.depth;
		});

	hits.reserve(collisions.size());
	for (int i = 0; i < collisions.size(); i++)
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

	return hits.size() != 0;
}

std::vector<Hit> PhysicsSystem::CircleCast(glm::vec2 pos, float radius, int layerMask, bool ignoreTriggers)
{
	std::vector<Hit> hits;

	for (auto& rigidBodyPair : *rigidBodiesInScene)
	{
		for (auto& collider : *rigidBodyPair.second.getColliders())
		{
			bool triggerPassing = true;
			if (collider->isTrigger && ignoreTriggers)
			{
				triggerPassing = false;
			}

			if (triggerPassing && (collider->collisionLayer & layerMask))
			{
				int type = (int)collider->getType();
				switch (type)
				{
				case (int)ColliderType::polygon:
					PolygonCollider* poly = (PolygonCollider*)collider;

					//Circle on Circle
					if (poly->verts.size() == 1)
					{
						glm::vec2 posB = RigidBody::Transform2Din3DSpace((*transformsInScene)[rigidBodyPair.first].getGlobalMatrix(), poly->verts[0]);
						float centreDistance = glm::length(posB - pos);
						float overlap = radius + poly->radius - centreDistance;

						if (overlap >= 0)
						{
							Hit hit;
							hit.collider = collider;
							hit.sceneObject = (*transformsInScene)[rigidBodyPair.first].getSceneObject();
							hits.push_back(hit);
						}
					}

					else
					{
						bool overLap = CollisionFunctions::CircleOnPolyCheck(
							pos, radius, poly, (*transformsInScene).at(rigidBodyPair.first)
						);
						if (overLap)
						{
							Hit hit;
							hit.collider = poly;
							hit.sceneObject = (*transformsInScene)[rigidBodyPair.first].getSceneObject();
							hits.push_back(hit);
						}
					}

					break;
				}
			}
		}

	}
	
	for (auto& colliderPair : *collidersInScene)
	{
		bool triggerPassing = true;
		if (colliderPair.second->isTrigger && ignoreTriggers)
		{
			triggerPassing = false;
		}

		if (triggerPassing && (colliderPair.second->collisionLayer & layerMask))
		{
			int type = (int)colliderPair.second->getType();
			switch (type)
			{
			case (int)ColliderType::polygon:
				PolygonCollider* poly = (PolygonCollider*)colliderPair.second;

				//Circle on Circle
				if (poly->verts.size() == 1)
				{
					glm::vec2 posB = RigidBody::Transform2Din3DSpace((*transformsInScene)[colliderPair.first].getGlobalMatrix(), poly->verts[0]);
					float centreDistance = glm::length(posB - pos);
					float overlap = radius + poly->radius - centreDistance;

					if (overlap >= 0)
					{
						Hit hit;
						hit.collider = colliderPair.second;
						hit.sceneObject = (*transformsInScene)[colliderPair.first].getSceneObject();
						hits.push_back(hit);
					}
				}

				else
				{
					bool overLap = CollisionFunctions::CircleOnPolyCheck(
						pos, radius, poly, (*transformsInScene)[colliderPair.first]
					);
					if (overLap)
					{
						Hit hit;
						hit.collider = poly;
						hit.sceneObject = (*transformsInScene)[colliderPair.first].getSceneObject();
						hits.push_back(hit);
					}
				}

				break;
			}
		}
	}
	return hits;
}

CollisionPacket PhysicsSystem::RayCastAgainstCollider(glm::vec2 pos, glm::vec2 direction, Transform& transform, Collider* collider, float radiusExpansion)
{
	// COLLISION BODY A is the target COLLISION BODY B SHOULD BE NULL
	CollisionPacket collision;
	collision.depth = -1.0f;
	if (rigidBodiesInScene == nullptr) return collision;


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
			float radius = poly->radius + radiusExpansion;
			float c = f.x * f.x + f.y * f.y - radius * radius;
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
				glm::vec2 a = pos;
				glm::vec2 b = direction;
				glm::vec2 c = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[i]);
				glm::vec2 d = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[(i + 1) % poly->verts.size()]) - c;

				glm::vec2 dRotated = { d.y, -d.x };

				float denominator = b.x * d.y - b.y * d.x;
				if (denominator <= 0) continue;
				//float numerator = a.y * b.x - b.y * a.x - c.y * b.x + b.y * c.x;
				float numerator = a.y * b.x + b.y * c.x - a.x * b.y - b.x * c.y;
				float t2 = numerator / denominator;
				if (t2 >= 0 && t2 <= 1)
				{
					float t1;
					if (abs(b.x) > abs(b.y))
					{
						t1 = c.x + d.x * t2 - a.x;
						t1 /= b.x;
					}
					else
					{
						t1 = c.y + d.y * t2 - a.y;
						t1 /= b.y;
					}

					if (t1 > 0.0f)
					{
						collision.contactPoint = a + b * t1;
						collision.colliderA = poly;
						collision.normal = glm::normalize(dRotated);
						collision.depth = glm::length(b * t1);
						collision.soA = transform.getSceneObject();
					}
				}
			}
		}
	}
	return collision;
}
