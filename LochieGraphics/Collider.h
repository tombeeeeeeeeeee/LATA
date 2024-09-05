#pragma once
#include "Maths.h"

#include "Serialisation.h"

#include <vector>

enum class CollisionLayers
{
	base = 1 << 0,
	wall = 1 << 1,
	enemy = 1 << 2,
	player = 1 << 3,
	reflectiveSurface = 1 << 4,
};

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
	int collisionLayer = 0;

	void setCollisionLayer(int layer);
	CollisionLayers getCollisionLayer();

	// TODO: Implement in children
	virtual toml::table Serialise(unsigned long long GUID) const;
	Collider* Load(toml::table table);
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
