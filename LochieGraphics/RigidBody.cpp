#include "RigidBody.h"

#include "Utilities.h"

#include "Serialisation.h"
#include "EditorGUI.h"
#include "LineRenderer.h"
#include "RenderSystem.h"
#include "Transform.h"
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

glm::vec2 RigidBody::AddDepen(glm::vec2 depenNormal, float depth)
{
	float amountAlreadyDepened = glm::dot(depenNormal, netDepen);
	float changeInNet = depth - amountAlreadyDepened;
	if (changeInNet > 0)
	{
		netDepen += depenNormal * changeInNet;
	}
	return netDepen;
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
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Rigid Body##" + tag).c_str()))
	{
		ImGui::Indent();
		float mass = getMass();
		bool isKinematic = invMass == 0;

		ImGui::BeginDisabled();
		ImGui::DragFloat2("Velocity", &vel[0]);
		ImGui::DragFloat2("Accel", &accel[0]);
		ImGui::DragFloat2("Depen", &netDepen[0]);
		ImGui::EndDisabled();
		if (ImGui::Button("Kill Movement")) vel = accel = {0.0f,0.0f};

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
		
		ImGui::Unindent();
	}
}

void RigidBody::DebugDraw(Transform* transform)
{
}

toml::table RigidBody::Serialise(unsigned long long GUID) const
{
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
}
