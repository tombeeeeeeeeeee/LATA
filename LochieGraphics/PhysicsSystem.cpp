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
	std::unordered_map<unsigned long long, Collider>& colliders

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

		for (auto k = colliders.begin(); k != colliders.end(); k++)
		{
			GetCollisions(
				&i->second, &k->second,
				&transforms[i->first], &transforms[k->first],
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

void PhysicsSystem::GetCollisions(RigidBody* a, Collider* b, Transform* transformA, Transform* transformB, std::vector<CollisionPacket> collisions)
{
	std::vector<Collider*> aCols = (*a->getColliders());
	Collider* bCol = b;

	for (Collider* colliderA : aCols)
	{
		if (colliderA->getType() == ColliderType::polygon && ((PolygonCollider*)colliderA)->verts.size() == 1)
		{
			if (bCol->getType() == ColliderType::polygon && ((PolygonCollider*)bCol)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnCircleCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)bCol,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
			else if (bCol->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)bCol,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
			else if (bCol->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)bCol,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
		}
		else if (colliderA->getType() == ColliderType::plane)
		{
			if (bCol->getType() == ColliderType::polygon && ((PolygonCollider*)bCol)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPlaneCollision(
					(PolygonCollider*)bCol, (PlaneCollider*)colliderA,
					&dumbyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
			else if (bCol->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)bCol, (PlaneCollider*)colliderA,
					&dumbyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
		}
		else if (colliderA->getType() == ColliderType::polygon)
		{
			if (bCol->getType() == ColliderType::polygon)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPolyCollision(
					(PolygonCollider*)colliderA, (PolygonCollider*)bCol,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
			else if (bCol->getType() == ColliderType::plane)
			{
				CollisionPacket collision = CollisionFunctions::PolyOnPlaneCollision(
					(PolygonCollider*)colliderA, (PlaneCollider*)bCol,
					a, &dumbyRigidBody, transformA, transformB
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
			else if (bCol->getType() == ColliderType::polygon && ((PolygonCollider*)bCol)->verts.size() == 1)
			{
				CollisionPacket collision = CollisionFunctions::CircleOnPolyCollision(
					(PolygonCollider*)bCol, (PolygonCollider*)colliderA,
					&dumbyRigidBody, a, transformB, transformA
				);
				if (collision.depth >= 0) collisions.push_back(collision);
			}
		}
		
	}
}

void PhysicsSystem::CollisisonResolution(CollisionPacket collision)
{
	if (collision.depth < 0) return;

	if (collision.rigidBodyA->invMass + collision.rigidBodyB->invMass == 0) return;

	glm::vec2 radiusPerpA = collision.tangentA;
	glm::vec2 radiusPerpB = collision.tangentB;

	float totalMass = collision.rigidBodyA->invMass + collision.rigidBodyB->invMass;

	collision.rigidBodyA->AddDepen(collision.normal * collision.depth * collision.rigidBodyA->invMass / totalMass);
	collision.rigidBodyB->AddDepen(-collision.normal * collision.depth * collision.rigidBodyB->invMass / totalMass);

	glm::vec2 relativeVelocity =
		(collision.rigidBodyA->vel + collision.rigidBodyA->angularVel * radiusPerpA)
		- (collision.rigidBodyB->vel + collision.rigidBodyB->angularVel * radiusPerpB);

	float totalInverseMass = (collision.rigidBodyA->invMass + collision.rigidBodyB->invMass);

	float j = -(1.0f + 0.65f) * glm::dot(relativeVelocity, collision.normal) /
		(totalInverseMass + pow(glm::dot(radiusPerpA, collision.normal), 2) * collision.rigidBodyA->invMomentOfInertia
			+ pow(glm::dot(radiusPerpB, collision.normal), 2) * collision.rigidBodyB->invMomentOfInertia);

	if (j <= 0) return;

	glm::vec2 linearRestitution = j * collision.normal;

	collision.rigidBodyA->AddImpulse(linearRestitution);
	collision.rigidBodyB->AddImpulse(-linearRestitution);

	if (abs(glm::dot(radiusPerpA, collision.normal)) > 0.000001f)
		collision.rigidBodyA->AddRotationalImpulse(glm::dot(radiusPerpA, linearRestitution));

	if (abs(glm::dot(radiusPerpB, collision.normal)) > 0.000001f)
		collision.rigidBodyB->AddRotationalImpulse(glm::dot(radiusPerpB, -linearRestitution));
}
