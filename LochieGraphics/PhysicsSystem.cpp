#include "PhysicsSystem.h"

void PhysicsSystem::UpdateRigidBodies(
	std::unordered_map<unsigned long long, Transform>& transforms,
	std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
	float deltaTime
)
{
	for (auto i = rigidBodies.begin(); i != rigidBodies.end(); i++)
	{
		if (i->second.isStatic) continue;

		glm::vec2 pos = i->second.getDepen();
		i->second.setAccel(i->second.getForce() * i->second.getInvMass());
		glm::vec2 vel =  i->second.AddVel(i->second.getAccel() * deltaTime);
		pos += vel * deltaTime;

		i->second.setDepen({ 0,0 });
		i->second.setForce({ 0,0 });

		transforms[i->first].setPosition(transforms[i->first].getPosition() + glm::vec3(pos.x, 0.0f, pos.y));
	}
}

void PhysicsSystem::CollisionCheckPhase(
	std::unordered_map<unsigned long long, Transform>& transforms,
	std::unordered_map<unsigned long long, RigidBody>& rigidBodies
)
{
	std::vector<CollisionPacket> collisions;
	for (auto i = rigidBodies.begin(); i != rigidBodies.end(); i++)
	{
		for (auto j = std::next(i); j != rigidBodies.end(); j++)
		{
			GetCollisions(
				&i->second, &j->second,
				&transforms[i->first], &transforms[j->first],
				collisions);
		}
	}

	for (int i = 0; i < collisions.size(); i++)
	{
		CollisisonResolution(collisions[i]);
	}
}

void PhysicsSystem::GetCollisions(
	RigidBody* a, RigidBody* b,
	Transform* transformA, Transform* transformB,
	std::vector<CollisionPacket> collisions)
{
	std::vector<Collider*> aCols = (*a->getColliders());
	std::vector<Collider*> bCols = (*b->getColliders());

	for (Collider* colliderA : aCols)
	{
		for (Collider* colliderB : bCols)
		{
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

void PhysicsSystem::CollisisonResolution(CollisionPacket collision)
{
	if (collision.depth < 0) return;

	if (collision.rigidBodyA->getInvMass() + collision.rigidBodyB->getInvMass() == 0) return;

	glm::vec2 radiusPerpA = collision.tangentA;
	glm::vec2 radiusPerpB = collision.tangentB;

	float totalMass = collision.rigidBodyA->getInvMass() + collision.rigidBodyB->getInvMass();

	collision.rigidBodyA->AddDepen(collision.normal * collision.depth * collision.rigidBodyA->getInvMass() / totalMass);
	collision.rigidBodyB->AddDepen(-collision.normal * collision.depth * collision.rigidBodyB->getInvMass() / totalMass);

	glm::vec2 relativeVelocity =
		(collision.rigidBodyA->getVel() + collision.rigidBodyA->getAngularVel() * radiusPerpA)
		- (collision.rigidBodyB->getVel() + collision.rigidBodyB->getAngularVel() * radiusPerpB);

	float totalInverseMass = (collision.rigidBodyA->getInvMass() + collision.rigidBodyB->getInvMass());

	float j = -(1 + 0.65) * glm::dot(relativeVelocity, collision.normal) /
		(totalInverseMass + pow(glm::dot(radiusPerpA, collision.normal), 2) * collision.rigidBodyA->getInvMomentOfInertia()
			+ pow(glm::dot(radiusPerpB, collision.normal), 2) * collision.rigidBodyB->getInvMomentOfInertia());

	if (j <= 0) return;

	glm::vec2 linearRestitution = j * collision.normal;

	collision.rigidBodyA->AddImpulse(linearRestitution);
	collision.rigidBodyB->AddImpulse(-linearRestitution);

	if (abs(glm::dot(radiusPerpA, collision.normal)) > 0.000001f)
		collision.rigidBodyA->AddRotationalImpulse(glm::dot(radiusPerpA, linearRestitution));

	if (abs(glm::dot(radiusPerpB, collision.normal)) > 0.000001f)
		collision.rigidBodyB->AddRotationalImpulse(glm::dot(radiusPerpB, -linearRestitution));
}
