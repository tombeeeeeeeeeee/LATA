#pragma once
#include "Maths.h"

#include <vector>

enum CollisionLayers 
{

};

class Collider;

class RigidBody
{
public:

	RigidBody();
	RigidBody(
		float _invMass,
		float _invMomentOfInertia,
		std::vector<Collider> _colliders = {},
		int _collisionLayer = 0
	) :
		invMass(_invMass),
		invMomentOfInertia(_invMomentOfInertia),
		colliders(_colliders),
		collisionLayer(_collisionLayer)
	{}

	void setAccel(glm::vec3 accel);
	glm::vec3 getAccel();
	glm::vec3 AddAccel(glm::vec3 accel);

	void setVel(glm::vec3 vel);
	glm::vec3 getVel();
	glm::vec3 AddVel(glm::vec3 vel);

	void setAngularVel(float angularVel);
	float getAngularVel();
	float addAngularVel(float angularVel);

	void setDepen(glm::vec3 depen);
	glm::vec3 getDepen();
	glm::vec3 AddDepen(glm::vec3 depen);

	void setCollisionLayer(int layer);
	CollisionLayers getCollisionLayer();

	void addCollider(Collider& collider);
	std::vector<Collider>* getColliders();

	void setInvMass(float invMass);
	void setMass(float mass);
	float getInvMass();
	float getMass();

	void setInvMomentOfInertia(float invMomentOfInertia);
	void setMomentOfInertia(float momentOfInertia);
	float getInvMomentOfInertia();
	float getMomentOfInertia();

private:
	glm::vec3 netAccel;
	glm::vec3 netDepen;

	glm::vec3 accel;
	glm::vec3 vel;

	float angularVel;
	float invMomentOfInertia = 0;
	float invMass = 0;

	std::vector<Collider> colliders;
	int collisionLayer;

	//TODO: ADD FUNCTION POINTERS FOR ON COLLISION AND ON TRIGGER
};