#include "CollisionFunctions.h"

#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"

#include "Utilities.h"
/*
    CollisionPacket collision = CollisionPacket();
    collision.rigidBodyA = rigidBodyA;
    collision.rigidBodyB = rigidBodyB;
	collision.soA = transformA->getSceneObject();
	collision.soB = transformB->getSceneObject();

    glm::vec2 posA = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), {0,0});
    glm::vec2 posB = RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix(), {0,0});


    return collision;
*/


CollisionPacket CollisionFunctions::CircleOnCircleCollision(
    PolygonCollider* circleA, PolygonCollider* circleB,
    RigidBody* rigidBodyA, RigidBody* rigidBodyB,
    Transform* transformA, Transform* transformB
)
{
    CollisionPacket collision = CollisionPacket();
    collision.rigidBodyA = rigidBodyA;
    collision.rigidBodyB = rigidBodyB;
	collision.colliderA = circleA;
	collision.colliderB = circleB;

    glm::vec2 posA = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), circleA->verts[0]);
    glm::vec2 posB = RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix(), circleB->verts[0]);

    float centreDistance = glm::length(posB - posA);
    float overlap = circleA->radius + circleB->radius - centreDistance;

    collision.depth = overlap;
	collision.normal = (centreDistance != 0) ? glm::normalize(posA - posB) : glm::vec2(1, 0);
    collision.contactPoint = posB + collision.normal * circleB->radius;

    collision.soA = transformA->getSceneObject();
    collision.soB = transformB->getSceneObject();

    return collision;
}

//TODO: add support for round polys
CollisionPacket CollisionFunctions::CircleOnPolyCollision(PolygonCollider* circle, PolygonCollider* poly, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
    CollisionPacket collision = CollisionPacket();
    collision.rigidBodyA = rigidBodyA;
    collision.rigidBodyB = rigidBodyB;
	collision.colliderA = circle;
	collision.colliderB = poly;
	collision.soA = transformA->getSceneObject();
	collision.soB = transformB->getSceneObject();

	std::vector<glm::vec2> polyPoints;
	for (int i = 0; i < poly->verts.size(); i++)
	{
		polyPoints.push_back(RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix() , poly->verts[i]));
	}

	float shortestDistance = FLT_MAX;
	glm::vec2 circlePos = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), circle->verts[0]);

	glm::vec2 prevVert = polyPoints[polyPoints.size() - 1];
	for (int i = 0; i < polyPoints.size(); i++)
	{
		glm::vec2 currVert = polyPoints[i];
		glm::vec2 delta = glm::normalize(circlePos - currVert);
		float polyMax = -FLT_MAX;
		float polyMin = FLT_MAX;
		float circleProj = glm::dot(circlePos, delta);
		for (int j = 0; j < polyPoints.size(); j++)
		{
			float vertProj = glm::dot(polyPoints[j], delta);
			if (vertProj > polyMax) polyMax = vertProj;
			if (vertProj < polyMin) polyMin = vertProj;
		}
		if (polyMax - (circleProj - circle->radius) < shortestDistance)
		{
			shortestDistance = polyMax - (circleProj - circle->radius);
			collision.normal = delta;
		}
		if ((circleProj + circle->radius) - polyMin < shortestDistance)
		{
			shortestDistance = (circleProj + circle->radius) - polyMin;
			collision.normal = -delta;
		}

		glm::vec2 tangent = currVert - prevVert;
		glm::vec2 normal = glm::normalize(glm::vec2(-tangent.y, tangent.x));
		polyMax = -FLT_MAX;
		polyMin = FLT_MAX;
		circleProj = glm::dot(circlePos, normal);
		for (int j = 0; j < polyPoints.size(); j++)
		{
			float vertProj = glm::dot(polyPoints[j], normal);
			if (vertProj > polyMax) polyMax = vertProj;
			if (vertProj < polyMin) polyMin = vertProj;
		}
		if (polyMax - (circleProj - circle->radius) < shortestDistance)
		{
			shortestDistance = polyMax - (circleProj - circle->radius);
			collision.normal = normal;
		}
		if ((circleProj + circle->radius) - polyMin < shortestDistance)
		{
			shortestDistance = (circleProj + circle->radius) - polyMin;
			collision.normal = -normal;
		}
		prevVert = currVert;
	}

	collision.depth = shortestDistance;
	if (shortestDistance  > 0)
	{
		float i = circle->radius;
	}

	return collision;
}

bool CollisionFunctions::CircleOnPolyCheck(glm::vec2 circlePos, float radius, PolygonCollider* poly, const Transform& transformB)
{
	bool collision = false;

	glm::vec2 polyTransformCentre = RigidBody::Transform2Din3DSpace(transformB.getGlobalMatrix(), { 0,0 });


	glm::mat4 globalB = transformB.getGlobalMatrix();
	std::vector<glm::vec2> polyPoints;
	polyPoints.reserve(poly->verts.size());
	for (int i = 0; i < poly->verts.size(); i++)
	{
		polyPoints.push_back(RigidBody::Transform2Din3DSpace(globalB, poly->verts[i]));
	}

	float shortestDistance = FLT_MAX;
	glm::vec2 closestPoint;
	int closestPointIndex = 0;

	for (int i = 0; i < polyPoints.size(); i++)
	{
		glm::vec2 pointToCircle = circlePos - polyPoints[i];
		float distance = glm::dot(pointToCircle, pointToCircle);
		if (distance < shortestDistance)
		{
			shortestDistance = distance;
			closestPoint = polyPoints[i];
			closestPointIndex = i;
		}
	}

	float minDepth = FLT_MAX;

	glm::vec2 curNormal = glm::normalize(circlePos - (closestPoint));
	//glm::vec2 curNormal = glm::vec2(-tangent.y, tangent.x);

	float circleDot = glm::dot(circlePos, curNormal);

	float circleMin = circleDot - radius;
	float circleMax = circleDot + radius;

	float polyMin = FLT_MAX;
	float polyMax = -FLT_MAX;

	for (int j = 0; j < polyPoints.size(); j++)
	{
		float curPolyDot = glm::dot(curNormal, polyPoints[j]);
		if (curPolyDot < polyMin)
			polyMin = curPolyDot;
		if (curPolyDot > polyMax)
			polyMax = curPolyDot;
	}

	if (circleMax - polyMin < minDepth)
	{
		minDepth = circleMax - polyMin;
	}

	if (polyMax - circleMin < minDepth)
	{
		minDepth = polyMax - circleMin;
	}

	glm::vec2 tangent = glm::normalize(polyPoints[(closestPointIndex + 1) % polyPoints.size()] - closestPoint);
	curNormal = glm::vec2(-tangent.y, tangent.x);

	float pointDepth = radius - glm::dot(circlePos, curNormal);
	pointDepth += glm::dot(polyPoints[closestPointIndex], curNormal);

	if (pointDepth < minDepth)
	{
		minDepth = pointDepth;
	}

	tangent = glm::normalize(closestPoint - polyPoints[Utilities::WrapIndex(closestPointIndex - 1, (int)polyPoints.size())]);
	curNormal = glm::vec2(-tangent.y, tangent.x);

	pointDepth = radius - glm::dot(circlePos, curNormal);
	pointDepth += glm::dot(polyPoints[closestPointIndex], curNormal);

	if (pointDepth < minDepth)
	{
		minDepth = pointDepth;
	}


	return minDepth >= 0;
}

//TODO:
CollisionPacket CollisionFunctions::CircleOnDirectionalPolyCollision(PolygonCollider* circle, DirectionalCollider directionalPoly, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
	return CollisionPacket();
}

CollisionPacket CollisionFunctions::CircleOnPlaneCollision(PolygonCollider* circle, PlaneCollider* plane, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
	CollisionPacket collision = CollisionPacket();
	collision.rigidBodyA = rigidBodyA;
	collision.rigidBodyB = rigidBodyB;
	collision.colliderA = circle;
	collision.colliderB = plane;
	collision.soA = transformA->getSceneObject();
	collision.soB = transformB->getSceneObject();

	glm::vec2 posA = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), { 0,0 });
	glm::vec2 posB = RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix(), { 0,0 });
	
	collision.depth = circle->radius - (glm::dot(posA + circle->verts[0], plane->normal) - plane->displacement);
	collision.normal = plane->normal;

	collision.contactPoint = posA + circle->verts[0] - plane->normal * circle->radius;

	collision.tangentA = collision.contactPoint - posA;
	collision.tangentA = { -collision.tangentA.y, collision.tangentA.x };

	return collision;
}

CollisionPacket CollisionFunctions::PolyOnPolyCollision(PolygonCollider* polyA, PolygonCollider* polyB, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
	CollisionPacket collision = CollisionPacket();
	collision.rigidBodyA = rigidBodyA;
	collision.rigidBodyB = rigidBodyB;
	collision.colliderA = polyA;
	collision.colliderB = polyB;
	collision.soA = transformA->getSceneObject();
	collision.soB = transformB->getSceneObject();

	glm::vec2 posA = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), { 0,0 });
	glm::vec2 posB = RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix(), { 0,0 });

	glm::mat4 globalA = transformA->getGlobalMatrix();
	std::vector<glm::vec2> polyAPoints;
	polyAPoints.reserve(polyA->verts.size());
	for (int i = 0; i < polyA->verts.size(); i++)
	{
		polyAPoints.push_back(RigidBody::Transform2Din3DSpace(globalA, polyA->verts[i]));
	}

	glm::mat4 globalB = transformB->getGlobalMatrix();
	std::vector<glm::vec2> polyBPoints;
	polyBPoints.reserve(polyB->verts.size());
	for (int i = 0; i < polyB->verts.size(); i++)
	{
		polyBPoints.push_back(RigidBody::Transform2Din3DSpace(globalB, polyB->verts[i]));
	}

	glm::vec2 normal = { 0,0 };
	float smallestDepth = FLT_MAX;
	glm::vec2 collisionPoint;

	for (int i = 0; i < polyAPoints.size(); i++)
	{
		glm::vec2 tangent = glm::normalize(polyAPoints[(i + 1) % polyAPoints.size()] - polyAPoints[i]);
		glm::vec2 curNormal = glm::vec2(-tangent.y, tangent.x);

		float aMax = glm::dot(curNormal, polyAPoints[i]);
		float bMin = FLT_MAX;


		for (int j = 0; j < polyBPoints.size(); j++)
		{
			float curBMin = glm::dot(curNormal, polyBPoints[j]);
			if (curBMin < bMin)
			{
				bMin = curBMin;
				collisionPoint = polyBPoints[j];
			}
		}

		float distanceToFirstPoint = glm::length((polyAPoints[i]) - posB);
		float distanceToSecondPoint = glm::length((polyAPoints[(i + 1) % polyAPoints.size()]) - posB);
		if (distanceToFirstPoint < glm::length(collisionPoint - posB)) collisionPoint = polyAPoints[i];
		if (distanceToSecondPoint < glm::length(collisionPoint - posB)) collisionPoint = polyAPoints[(i + 1) % polyAPoints.size()];

		if (aMax - bMin < smallestDepth)
		{
			normal = -curNormal;
			collision.contactPoint = collisionPoint;
			smallestDepth = aMax - bMin;
		}
	}
	for (int i = 0; i < polyBPoints.size(); i++)
	{
		glm::vec2 tangent = glm::normalize(polyBPoints[(i + 1) % polyBPoints.size()] - polyBPoints[i]);
		glm::vec2 curNormal = glm::vec2(-tangent.y, tangent.x);

		float bMax = glm::dot(curNormal, polyBPoints[i]);

		float aMin = FLT_MAX;
		for (int j = 0; j < polyAPoints.size(); j++)
		{
			float curAMin = glm::dot(curNormal, polyAPoints[j]);
			if (curAMin < aMin)
			{
				aMin = curAMin;
				collisionPoint = polyAPoints[j];
			}
		}

		float distanceToFirstPoint = glm::length((polyBPoints[i]) - posA);
		float distanceToSecondPoint = glm::length((polyBPoints[(i + 1) % polyBPoints.size()]) - posA);
		if (distanceToFirstPoint < glm::length(collisionPoint - posA)) collisionPoint = (polyBPoints[i]);
		if (distanceToSecondPoint < glm::length(collisionPoint - posA)) collisionPoint = (polyBPoints[(i + 1) % polyBPoints.size()]);

		if (bMax - aMin < smallestDepth)
		{
			normal = curNormal;
			collision.contactPoint = collisionPoint;
			smallestDepth = bMax - aMin;
		}
	}

	collision.depth = smallestDepth;
	collision.normal = normal;

	collision.tangentA = collision.contactPoint - posA;
	collision.tangentA = { -collision.tangentA.y, collision.tangentA.x };

	return collision;
}

//TODO:
CollisionPacket CollisionFunctions::PolyOnDirectionalPolyCollision(PolygonCollider* poly, DirectionalCollider* directionalPoly, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
	return CollisionPacket();
}

CollisionPacket CollisionFunctions::PolyOnPlaneCollision(PolygonCollider* poly, PlaneCollider* plane, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
	CollisionPacket collision = CollisionPacket();
	collision.rigidBodyA = rigidBodyA;
	collision.rigidBodyB = rigidBodyB;
	collision.colliderA = poly;
	collision.colliderB = plane;
	collision.soA = transformA->getSceneObject();
	collision.soB = transformB->getSceneObject();

	glm::vec2 posA = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), { 0,0 });
	glm::vec2 posB = RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix(), { 0,0 });

	glm::mat4 globalA = transformA->getGlobalMatrix();
	std::vector<glm::vec2> points;
	points.reserve(poly->verts.size());
	for (int i = 0; i < poly->verts.size(); i++)
	{
		points.push_back(RigidBody::Transform2Din3DSpace(globalA, poly->verts[i]));
	}

	float depth = -FLT_MAX;
	int pointIndex = 0;
	for (int i = 0; i < points.size(); i++)
	{
		float pointDepth = -glm::dot(points[i], plane->normal) + plane->displacement;
		if (pointDepth > depth)
		{
			depth = pointDepth;
			pointIndex = i;
		}
	}

	collision.depth = depth;
	collision.normal = plane->normal;

	collision.contactPoint = points[pointIndex];

	collision.tangentA = collision.contactPoint - posA;
	collision.tangentA = {-collision.tangentA.y, collision.tangentA.x};

	return collision;
}

//TODO: 
CollisionPacket CollisionFunctions::DirectionalPolyOnPlaneCollision(DirectionalCollider* directionalPoly, PlaneCollider* plane, RigidBody* rigidBodyA, RigidBody* rigidBodyB, Transform* transformA, Transform* transformB)
{
	return CollisionPacket();
}

glm::vec2 CollisionFunctions::SATMinMax(glm::vec2 axis, std::vector<glm::vec2> verts)
{
	// note that we project the first point to both min and max
	float min = glm::dot(axis, verts[0]);
	float max = min;

	// now we loop over the remiaing vers, updating min/max as required
	for (int j = 1; j < verts.size(); j++)
	{
		float temp = glm::dot(axis, verts[j]);
		if (temp < min) min = temp;
		if (temp > max) max = temp;
	}

	return { min, max };
}
