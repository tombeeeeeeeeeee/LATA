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

	glm::vec2 circleTransformCentre = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), { 0,0 });
	glm::vec2 polyTransformCentre = RigidBody::Transform2Din3DSpace(transformB->getGlobalMatrix(), { 0,0 });


	glm::mat4 globalB = transformB->getGlobalMatrix();
	std::vector<glm::vec2> polyPoints;
	polyPoints.reserve(poly->verts.size());
	for (int i = 0; i < poly->verts.size(); i++)
	{
		polyPoints.push_back(RigidBody::Transform2Din3DSpace(globalB, poly->verts[i]));
	}

	float shortestDistance = FLT_MAX;
	glm::vec2 delta =  polyTransformCentre - circleTransformCentre;
	glm::vec2 closestPoint;
	glm::vec2 circlePos = RigidBody::Transform2Din3DSpace(transformA->getGlobalMatrix(), circle->verts[0]);

	for (int i = 0; i < polyPoints.size(); i++)
	{
		glm::vec2 pointToCircle = circlePos - polyPoints[i];
		float distance = glm::dot(pointToCircle, pointToCircle);
		if (distance < shortestDistance)
		{
			shortestDistance = distance;
			closestPoint = polyPoints[i];
		}
	}

	glm::vec2 axis = glm::normalize(closestPoint - circlePos);

	glm::vec2 minMax = SATMinMax(axis, polyPoints);
	minMax += glm::dot(axis, delta);
	/*	
		let vOffset = new SATPoint(polygon.x - circle.x, polygon.y - circle.y);



		// calculate the axis from the circle to the point
		let axis = new SATPoint(closestVertex.x - circle.x, closestVertex.y - circle.y);
		axis.normalize();

		// project the polygon onto this axis
		let polyRange = SAT._projectVertsForMinMax(axis, verts);

		// shift the polygon along the axis
		var scalerOffset = SAT._vectorDotProduct(axis, vOffset);
		polyRange.min += scalerOffset;
		polyRange.max += scalerOffset;

		// project the circle onto this axis
		let circleRange = this._projectCircleForMinMax(axis, circle);

		// if there is a gap then bail now
		if ( (polyRange.min - circleRange.max > 0) || (circleRange.min - polyRange.max > 0)  )
		{
			// there is a gap - bail
			return null;
		}


		// calc the separation and store if this is the shortest
		let distMin = (circleRange.max - polyRange.min);
		if (flipResultPositions) distMin *= -1;

		// store this as the shortest distances because it is the first
		shortestDist = Math.abs(distMin);

		result.distance = distMin;
		result.vector = axis;

		// check for containment
		this._checkRangesForContainment(polyRange, circleRange, result, flipResultPositions);


		// now loop over the polygon sides and do a similar thing
		for (let i = 0; i < verts.length; i++)
		{
			// get the perpendicular axis that we will be projecting onto
			axis = SAT._getPerpendicularAxis(verts, i);
			// project each point onto the axis and circle
			polyRange = SAT._projectVertsForMinMax(axis, verts);

			// shift the first polygons min max along the axis by the amount of offset between them
			var scalerOffset = SAT._vectorDotProduct(axis, vOffset);
			polyRange.min += scalerOffset;
			polyRange.max += scalerOffset;

			// project the circle onto this axis
			circleRange = this._projectCircleForMinMax(axis, circle);

			// now check for a gap betwen the relative min's and max's
			if ( (polyRange.min - circleRange.max > 0) || (circleRange.min - polyRange.max > 0)  )
			{
				// there is a gap - bail
				return null;
			}

			// check for containment
			this._checkRangesForContainment(polyRange, circleRange, result, flipResultPositions);

			distMin = (circleRange.max - polyRange.min);// * -1;
			if (flipResultPositions) distMin *= -1;

			// check if this is the shortest by using the absolute val
			let distMinAbs = Math.abs(distMin);
			if (distMinAbs < shortestDist)
			{
				shortestDist = distMinAbs;

				result.distance = distMin;
				result.vector = axis;
			}
		}

		// calc the final separation
		result.separation = new SATPoint(result.vector.x * result.distance, result.vector.y * result.distance);

		// if you make it here then no gaps were found
		return result;
	
	
	
	*/


	//Find Closest Vert on Poly to Circle

	float closestPointDistance = FLT_MAX;
	int closestPointIndex = 0;
	for (int i = 0; i < poly->verts.size(); i++)
	{
		float curPointDistance = glm::length(polyPoints[i] - circlePos);
		if (curPointDistance < closestPointDistance)
		{
			closestPointIndex = i;
			closestPointDistance = curPointDistance;
		}
	}

	float minDepth = FLT_MAX;

	glm::vec2 tangent = glm::normalize(circlePos - (polyPoints[closestPointIndex]));
	glm::vec2 curNormal = glm::vec2(-tangent.y, tangent.x);

	float circleDot = glm::dot(circlePos, tangent);

	float circleMin = circleDot - circle->radius;
	float circleMax = circleDot + circle->radius;

	float polyMin = FLT_MAX;
	float polyMax = -FLT_MAX;

	for (int j = 0; j < polyPoints.size(); j++)
	{
		float curPolyDot = glm::dot(tangent, polyPoints[j]);
		if (curPolyDot < polyMin)
			polyMin = curPolyDot;
		if (curPolyDot > polyMax)
			polyMax = curPolyDot;
	}

	if (circleMax - polyMin < minDepth)
	{
		minDepth = circleMax - polyMin;
		collision.contactPoint = polyPoints[closestPointIndex];
		collision.normal = tangent;
	}

	if (polyMax - circleMin < minDepth)
	{
		minDepth = polyMax - circleMin;
		collision.contactPoint = polyPoints[closestPointIndex];
		collision.normal = tangent;
	}

	tangent = glm::normalize(polyPoints[(closestPointIndex + 1) % polyPoints.size()] - polyPoints[closestPointIndex]);
	curNormal = glm::vec2(-tangent.y, tangent.x);

	float pointDepth = circle->radius - glm::dot(circlePos, curNormal);
	pointDepth += glm::dot(polyPoints[closestPointIndex], curNormal);

	if (pointDepth < minDepth)
	{
		minDepth = pointDepth;
		collision.normal = curNormal;

		collision.contactPoint = curNormal * -circle->radius;
		collision.contactPoint += circlePos;
	}

	tangent = glm::normalize(polyPoints[closestPointIndex] - polyPoints[Utilities::WrapIndex(closestPointIndex - 1, (int)polyPoints.size())]);
	curNormal = glm::vec2(-tangent.y, tangent.x);

	pointDepth = circle->radius - glm::dot(circleTransformCentre + circle->verts[0], curNormal);
	pointDepth += glm::dot(polyPoints[closestPointIndex], curNormal);

	if (pointDepth < minDepth)
	{
		minDepth = pointDepth;
		collision.normal = curNormal;

		collision.contactPoint = curNormal * -circle->radius;
		collision.contactPoint += circlePos;
	}

	collision.depth = minDepth;

	collision.tangentA = collision.contactPoint - circleTransformCentre;
	collision.tangentA = { -collision.tangentA.y, collision.tangentA.x };

	return collision;
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
