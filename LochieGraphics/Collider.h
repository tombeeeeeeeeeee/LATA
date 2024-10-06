#pragma once
#include "Maths.h"

#include <vector>

namespace toml {
	inline namespace v3 {
		class table;
	}
}

enum class CollisionLayers
{
	base = 1 << 0,
	enemy = 1 << 1,
	reflectiveSurface = 1 << 2,
	sync = 1 << 3,
	ecco = 1 << 4,
	syncProjectile = 1 << 5,
	eccoProjectile = 1 << 6,
	enemyProjectile = 1 << 7,
	softCover = 1 << 8,
	halfCover = 1 << 9,
	count = 1 << 10
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
	virtual void GUI();

	static const int transparentLayers = (int)CollisionLayers::syncProjectile | (int)CollisionLayers::eccoProjectile | (int)CollisionLayers::enemyProjectile | (int)CollisionLayers::softCover | (int)CollisionLayers::halfCover;

protected:
	Collider(toml::table table);
};

struct PolygonCollider : public Collider
{
	std::vector<glm::vec2> verts;
	float radius = 0.0f;
	virtual const ColliderType getType() const { return ColliderType::polygon; };
	PolygonCollider(std::vector<glm::vec2> _verts, float _radius, CollisionLayers layer = CollisionLayers::base) : verts(_verts), radius(_radius) { collisionLayer = (int)layer; };
	PolygonCollider() {};
	virtual toml::table Serialise(unsigned long long GUID) const;
	PolygonCollider(toml::table table);
	void GUI() override;
};

struct PlaneCollider : public Collider
{
	glm::vec2 normal;
	float displacement;
	virtual const ColliderType getType() const { return ColliderType::plane; };
	virtual toml::table Serialise(unsigned long long GUID) const;
	PlaneCollider(toml::table table);
	void GUI() override;
};

struct DirectionalCollider : public PolygonCollider
{
	std::vector<int> collidingFaces;
	virtual const ColliderType getType() const { return ColliderType::directionalPoly; };
	virtual toml::table Serialise(unsigned long long GUID) const;
	DirectionalCollider(toml::table table);
	void GUI() override;
};
