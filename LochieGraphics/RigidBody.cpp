#include "RigidBody.h"
#include "Utilities.h"

#include "imgui.h"

#include <string>
#include <iostream>

RigidBody::RigidBody()
{
}

glm::vec2 RigidBody::AddImpulse(glm::vec2 impulse)
{
	vel += impulse * invMass;
	return vel;
}

float RigidBody::AddRotationalImpulse(float impulse)
{
	angularVel += invMomentOfInertia * impulse;
	return angularVel;
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

void RigidBody::addCollider(Collider* collider)
{
	colliders.push_back(collider);
}

std::vector<Collider*>* RigidBody::getColliders()
{
	return &colliders;
}

void RigidBody::setMass(float mass)
{
	float temp = glm::max(mass, 0.0f);
	invMass = 1 / temp;
}


float RigidBody::getMass()
{
	return 1/invMass;
}

void RigidBody::setMomentOfInertia(float momentOfInertia)
{
	invMomentOfInertia = 1/ glm::max(momentOfInertia, 0.0f);
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

void RigidBody::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	float mass = getMass();
	bool isKinematic = invMass == 0;

	if (isKinematic) ImGui::BeginDisabled();
	if (ImGui::DragFloat(("Mass##rigidBody" + tag).c_str(), &mass, 0.1f, 0.001f, 100000.0f))
	{
		setMass(mass);
	}
	if (isKinematic) ImGui::EndDisabled();

	if (ImGui::Checkbox(("Kinematic##rigidBody" + tag).c_str(), &isKinematic))
	{
		if (isKinematic)
			invMass = 0.0f;
		else
			setMass(1.0f);
	}
	ImGui::Checkbox(("Static##rigidBody" + tag).c_str(), &isStatic);
	ImGui::DragFloat(("Elasticity##rigidBody" + tag).c_str(), &elasticicty, 0.01f, 0.0f, 1.0f);
	//TODO: add collider gui
}
