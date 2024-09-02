#pragma once

#include "Collision.h"
#include "Collider.h"

#include <vector>
#include <functional>

#include "Serialisation.h"

class RigidBody
{
public:

	RigidBody();
	RigidBody(
		float _invMass,
		float _invMomentOfInertia,
		std::vector<Collider*> _colliders = {},
		bool _isStatic = false
	) :
		invMass(_invMass),
		invMomentOfInertia(_invMomentOfInertia),
		colliders(_colliders),
		isStatic(_isStatic)
	{}

	glm::vec2 netForce = glm::zero<glm::vec2>();
	glm::vec2 netDepen = glm::zero<glm::vec2>();

	glm::vec2 accel = glm::zero<glm::vec2>();
	glm::vec2 vel = glm::zero<glm::vec2>();

	float angularVel = 0;
	float invMomentOfInertia = 0;
	float invMass = 0;

	float elasticicty = 0;

	std::vector<Collider*> colliders = {};

	glm::vec2 AddImpulse(glm::vec2 impulse);
	float AddRotationalImpulse(float impulse);

	glm::vec2 AddDepen(glm::vec2 depen);

	void addCollider(Collider* collider);
	std::vector<Collider*>* getColliders();

	void setMass(float mass);
	float getMass();

	void setMomentOfInertia(float momentOfInertia);
	float getMomentOfInertia();

	bool isStatic = false;

	std::vector<std::function<void(Collision)>> onTrigger;
	std::vector<std::function<void(Collision)>> onCollision;

	static glm::vec2 Transform2Din3DSpace(glm::mat4 global, glm::vec2 input);

	void GUI();

	toml::table Serialise(unsigned long long GUID) const;

private:
	//TODO: ADD FUNCTION POINTERS FOR ON COLLISION AND ON TRIGGER
};