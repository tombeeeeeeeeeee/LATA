#include "RigidBody.h"

RigidBody::RigidBody()
{
}

glm::vec2 RigidBody::AddImpulse(glm::vec2 impulse)
{
	return vel += impulse * invMass;
}

float RigidBody::AddRotationalImpulse(float impulse)
{
	return angularVel += invMomentOfInertia * impulse;
}

void RigidBody::setForce(glm::vec2 force)
{
	netForce = force;
}

glm::vec2 RigidBody::getForce()
{
	return netForce;
}

glm::vec2 RigidBody::addForce(glm::vec2 force)
{
	return netForce += force;
}

void RigidBody::setAccel(glm::vec2 _accel)
{
	accel = _accel;
}

glm::vec2 RigidBody::getAccel()
{
	return accel;
}

glm::vec2 RigidBody::AddAccel(glm::vec2 _accel)
{
	return accel += _accel;
}

void RigidBody::setVel(glm::vec2 _vel)
{
	vel = _vel;
}

glm::vec2 RigidBody::getVel()
{
	return vel;
}

glm::vec2 RigidBody::AddVel(glm::vec2 _vel)
{
	vel += _vel;
	return vel;
}

void RigidBody::setAngularVel(float _angularVel)
{
	angularVel = _angularVel;
}

float RigidBody::getAngularVel()
{
	return angularVel;
}

float RigidBody::addAngularVel(float _angularVel)
{
	angularVel += _angularVel;
	return angularVel;
}

void RigidBody::setDepen(glm::vec2 depen)
{
	netDepen = depen;
}

glm::vec2 RigidBody::getDepen()
{
	return netDepen;
}

glm::vec2 RigidBody::AddDepen(glm::vec2 depen)
{
	if (abs(depen.x) + abs(depen.y) <= 0.000001f) return netDepen;
	if (glm::dot(depen, netDepen) <= 0.000001f)
	{
		netDepen = depen + netDepen;
	}
	else
	{
		glm::vec2 normalNet = glm::normalize(netDepen);
		float amountAlreadyDepened = glm::dot(normalNet, depen);
		glm::vec2 changeInNet = netDepen - amountAlreadyDepened * normalNet;
		if (glm::dot(changeInNet, netDepen) < 0)
		{
			netDepen = depen;
		}
		else netDepen = changeInNet + depen;
	}
	return netDepen;
}

void RigidBody::setCollisionLayer(int layer)
{
	collisionLayer = layer;
}

CollisionLayers RigidBody::getCollisionLayer()
{
	return (CollisionLayers)collisionLayer;
}

void RigidBody::addCollider(Collider* collider)
{
	colliders.push_back(collider);
}

std::vector<Collider*>* RigidBody::getColliders()
{
	return &colliders;
}

void RigidBody::setInvMass(float _invMass)
{
	invMass = glm::max(_invMass, 0.0f);
}

void RigidBody::setMass(float mass)
{
	float temp = glm::max(mass, 0.0f);
	invMass = 1 / temp;
}

float RigidBody::getInvMass()
{
	return invMass;
}

float RigidBody::getMass()
{
	return 1/invMass;
}

void RigidBody::setInvMomentOfInertia(float _invMomentOfInertia)
{
	invMomentOfInertia = glm::max(_invMomentOfInertia, 0.0f);
}

void RigidBody::setMomentOfInertia(float momentOfInertia)
{
	invMomentOfInertia = 1/ glm::max(momentOfInertia, 0.0f);
}

float RigidBody::getInvMomentOfInertia()
{
	return invMomentOfInertia;
}

float RigidBody::getMomentOfInertia()
{
	return 1/invMomentOfInertia;
}

glm::vec2 RigidBody::Transform2Din3DSpace(glm::mat4 global, glm::vec2 input)
{
	glm::vec4 temp = { input.x, 0.0f, input.y, 1.0f };
	glm::vec3 result = global * temp;
	return { result.x, result.z };
}
