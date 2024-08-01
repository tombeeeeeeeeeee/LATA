#pragma once
#include "Maths.h"
#include "Collider.h"
#include <vector>

enum CollisionLayers 
{
	base
};

class RigidBody
{
public:

	RigidBody();
	RigidBody(
		float _invMass,
		float _invMomentOfInertia,
		std::vector<Collider> _colliders = {},
		int _collisionLayer = 0,
		bool _isStatic = false
	) :
		invMass(_invMass),
		invMomentOfInertia(_invMomentOfInertia),
		colliders(_colliders),
		collisionLayer(_collisionLayer),
		isStatic(_isStatic)
	{}

	glm::vec2 AddImpulse(glm::vec2 impulse);
	float AddRotationalImpulse(float impulse);

	void setForce(glm::vec2 force);
	glm::vec2 getForce();
	glm::vec2 addForce(glm::vec2 force);

	void setAccel(glm::vec2 accel);
	glm::vec2 getAccel();
	glm::vec2 AddAccel(glm::vec2 accel);

	void setVel(glm::vec2 vel);
	glm::vec2 getVel();
	glm::vec2 AddVel(glm::vec2 vel);

	void setAngularVel(float angularVel);
	float getAngularVel();
	float addAngularVel(float angularVel);

	void setDepen(glm::vec2 depen);
	glm::vec2 getDepen();
	glm::vec2 AddDepen(glm::vec2 depen);

	void setCollisionLayer(int layer);
	CollisionLayers getCollisionLayer();

	void addCollider(Collider* collider);
	std::vector<Collider*>* getColliders();

	void setInvMass(float invMass);
	void setMass(float mass);
	float getInvMass();
	float getMass();

	void setInvMomentOfInertia(float invMomentOfInertia);
	void setMomentOfInertia(float momentOfInertia);
	float getInvMomentOfInertia();
	float getMomentOfInertia();

	bool isStatic = false;

	static glm::vec2 Transform2Din3DSpace(glm::mat4 global, glm::vec2 input);

private:
	glm::vec2 netForce = glm::zero<glm::vec2>();
	glm::vec2 netDepen = glm::zero<glm::vec2>();
			
	glm::vec2 accel = glm::zero<glm::vec2>();
	glm::vec2 vel = glm::zero<glm::vec2>();

	float angularVel = 0;
	float invMomentOfInertia = 0;
	float invMass = 0;

	std::vector<Collider*> colliders = {};
	int collisionLayer = 0;

	//TODO: ADD FUNCTION POINTERS FOR ON COLLISION AND ON TRIGGER
};