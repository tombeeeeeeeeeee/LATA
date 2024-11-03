#pragma once

#include "Maths.h"

#include <vector>
#include <functional>

struct Collision;
struct Collider;
class Transform;
class LineRenderer;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

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
	float maxVertDistance = 0.0f;

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
	void DebugDraw(Transform* transform);

	toml::table Serialise(unsigned long long GUID) const;
	RigidBody(toml::table table);

	bool ignoreThisCollision = false;
private:
	std::vector<Collider*> colliders = {};
	
};