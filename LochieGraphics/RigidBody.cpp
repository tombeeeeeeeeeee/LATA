#include "RigidBody.h"

#include "Utilities.h"

#include "EditorGUI.h"

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
	return 1 / invMass;
}

void RigidBody::setMomentOfInertia(float momentOfInertia)
{
	invMomentOfInertia = 1 / glm::max(momentOfInertia, 0.0f);
}

float RigidBody::getMomentOfInertia()
{
	return 1 / invMomentOfInertia;
}

glm::vec2 RigidBody::Transform2Din3DSpace(glm::mat4 global, glm::vec2 input)
{
	glm::vec4 temp = { input.x, 0.0f, input.y, 1.0f };
	glm::vec4 result = global * temp;
	return glm::vec2(result.x, result.z);
}

void RigidBody::GUI()
{
	if (ImGui::CollapsingHeader("Rigid Body"))
	{
		ImGui::Indent();
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

		for (auto i = 0; i < colliders.size(); i++)
		{
			if (ImGui::CollapsingHeader(("Collider " + std::to_string(i) + "##" + tag).c_str())) {
				colliders.at(i)->GUI();
			}
		}
		ImGui::Unindent();
	}
}

toml::table RigidBody::Serialise(unsigned long long GUID) const
{
	toml::array savedColliders;
	for (auto i : colliders)
	{
		savedColliders.push_back(i->Serialise(GUID));
	}
	// TODO: Ensure function pointers are safe
	return toml::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID) },
		{ "netForce", Serialisation::SaveAsVec2(netForce) },
		{ "netDepen", Serialisation::SaveAsVec2(netDepen) },
		{ "accel", Serialisation::SaveAsVec2(accel)},
		{ "vel", Serialisation::SaveAsVec2(vel)},
		{ "angularVel", angularVel },
		{ "invMomentOfInertia", invMomentOfInertia},
		{ "invMass", invMass},
		{ "elasticicty", elasticicty},
		{ "colliders", savedColliders},
		{ "isStatic", isStatic},
	};
}

RigidBody::RigidBody(toml::table table)
{
	netForce = Serialisation::LoadAsVec2(table["netForce"]);
	netDepen = Serialisation::LoadAsVec2(table["netDepen"]);
	accel = Serialisation::LoadAsVec2(table["accel"]);
	vel = Serialisation::LoadAsVec2(table["vel"]);
	angularVel = Serialisation::LoadAsFloat(table["angularVel"]);
	invMomentOfInertia = Serialisation::LoadAsFloat(table["invMomentOfInertia"]);
	invMass = Serialisation::LoadAsFloat(table["invMass"]);
	elasticicty = Serialisation::LoadAsFloat(table["elasticicty"]);
	isStatic = Serialisation::LoadAsBool(table["isStatic"]);
	toml::array* loadingColliders = table["colliders"].as_array();
	if (!loadingColliders) {
		colliders.resize(0);
		return;
	}
	for (size_t i = 0; i < loadingColliders->size(); i++)
	{
		colliders.push_back(Collider::Load(*loadingColliders->at(i).as_table()));
	}
}
