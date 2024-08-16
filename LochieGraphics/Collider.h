#pragma once
#include "Maths.h"
#include <vector>

enum class ColliderType
{
	empty,
	polygon,
	plane,
	directionalPoly,
};

struct Collider
{
	virtual const ColliderType getType() { return ColliderType::empty; };
	bool isTrigger = false;
};

struct PolygonCollider : public Collider
{
	std::vector<glm::vec2> verts;
	float radius;
	virtual const ColliderType getType() { return ColliderType::polygon; };
	PolygonCollider(std::vector<glm::vec2> _verts, float _radius) : verts(_verts), radius(_radius) {};
	PolygonCollider() {};
};

struct PlaneCollider : public Collider
{
	glm::vec2 normal;
	float displacement;
	virtual const ColliderType getType() { return ColliderType::plane; };
};

struct DirectionalCollider : public PolygonCollider
{
	std::vector<int> collidingFaces;
	virtual const ColliderType getType() { return ColliderType::directionalPoly; };
};
