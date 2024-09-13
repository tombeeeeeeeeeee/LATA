#pragma once
#include "Maths.h"

#include "Serialisation.h"

#include <vector>

enum class CollisionLayers
{
	base = 1 << 0,
	enemy = 1 << 1,
	reflectiveSurface = 1 << 2,
	sync = 1 << 3,
	ecco = 1 << 4,
	syncProjectile = 1 << 5,
	eccoProjectile = 1 << 6,
	ignoreRaycast = 1 << 7,
	count = 1 << 8
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
	virtual const ColliderType getType() const { return ColliderType::empty; };
	bool isTrigger = false;
	int collisionLayer = 0;

	void setCollisionLayer(int layer);
	CollisionLayers getCollisionLayer();

	virtual toml::table Serialise(unsigned long long GUID) const;
	static Collider* Load(toml::table table);
	Collider() = default;
protected:
	Collider(toml::table table);
};

struct PolygonCollider : public Collider
{
	std::vector<glm::vec2> verts;
	float radius;
	virtual const ColliderType getType() const { return ColliderType::polygon; };
	PolygonCollider(std::vector<glm::vec2> _verts, float _radius, CollisionLayers layer = CollisionLayers::base) : verts(_verts), radius(_radius) { collisionLayer = (int)layer; };
	PolygonCollider() {};
	virtual toml::table Serialise(unsigned long long GUID) const;
	PolygonCollider(toml::table table);
};

struct PlaneCollider : public Collider
{
	glm::vec2 normal;
	float displacement;
	virtual const ColliderType getType() const { return ColliderType::plane; };
	virtual toml::table Serialise(unsigned long long GUID) const;
	PlaneCollider(toml::table table);
};

struct DirectionalCollider : public PolygonCollider
{
	std::vector<int> collidingFaces;
	virtual const ColliderType getType() const { return ColliderType::directionalPoly; };
	virtual toml::table Serialise(unsigned long long GUID) const;
	DirectionalCollider(toml::table table);
};
