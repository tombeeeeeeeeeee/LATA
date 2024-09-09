#include "Sync.h"

#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"

#include "EditorGUI.h"

Sync::Sync(toml::table table)
{
	GUID = Serialisation::LoadAsUnsignedLongLong(table["guid"]);
	moveSpeed = Serialisation::LoadAsFloat(table["moveSpeed"]);
	lookDeadZone = Serialisation::LoadAsFloat(table["lookDeadZone"]);
	moveDeadZone = Serialisation::LoadAsFloat(table["moveDeadZone"]);
	misfireDamage = Serialisation::LoadAsFloat(table["misfireDamage"]);
	sniperDamage = Serialisation::LoadAsFloat(table["sniperDamage"]);
	overclockDamage = Serialisation::LoadAsFloat(table["overclockChargeTime"]);
}

void Sync::Update(Input::InputDevice& inputDevice, Transform& transform, RigidBody& rigidBody, float delta)
{
	glm::vec2 look = inputDevice.getLook();
	glm::vec2 move = inputDevice.getMove();
	if (glm::length(look) > lookDeadZone)
	{
		float turnAmount = glm::dot(transform.forward(), glm::normalize(glm::vec3(look.x, 0.0f, look.y)));
		float desiredAngle = glm::acos(turnAmount);
		glm::vec3 eulers = transform.getEulerRotation();
		transform.setEulerRotation({ eulers.x, eulers.y + desiredAngle, eulers.z });
	}
	if (glm::length(move) > moveDeadZone)
		rigidBody.vel = moveSpeed * move;
	else
		rigidBody.vel = { 0.0f, 0.0f };

	if (inputDevice.getRightTrigger())
	{

	}
}

void Sync::GUI()
{
	//ImGui::Text("");
	if (ImGui::CollapsingHeader("Sync Component"))
	{
		ImGui::DragFloat("Move Speed", &moveSpeed);
		ImGui::DragFloat("Look DeadZone", &lookDeadZone);
		ImGui::DragFloat("Move DeadZone", &moveDeadZone);
		ImGui::DragFloat("Misfire Damage", &misfireDamage);
		ImGui::DragFloat("Sniper Damage", &sniperDamage);
		ImGui::DragFloat("Overclock Damage", &overclockDamage);
		ImGui::DragFloat("Sniper Charge Time", &sniperChargeTime);
		ImGui::DragFloat("Overlock Charge Time", &overclockChargeTime);

	}
}

toml::table Sync::Serialise() const
{
	return toml::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "moveSpeed", moveSpeed},
		{ "lookDeadZone", lookDeadZone},
		{ "moveDeadZone", moveDeadZone},
		{ "misfireDamage", misfireDamage},
		{ "sniperDamage", sniperDamage},
		{ "overclockChargeTime", overclockChargeTime},
	};
}

void Sync::misfireShotOnCollision(Collision collision)
{
	if (collision.collisionMask & (unsigned int)CollisionLayers::enemy)
	{
		collision.sceneObject->health()->subtractHealth(misfireDamage);
	}
}

void Sync::sniperShotOnCollision(Collision collision)
{
	if (collision.collisionMask & (unsigned int)CollisionLayers::enemy)
	{
		collision.sceneObject->health()->subtractHealth(sniperDamage);
	}
}

void Sync::overclockShotOnCollision(Collision collision)
{
	if (collision.collisionMask & (unsigned int)CollisionLayers::enemy)
	{
		collision.sceneObject->health()->subtractHealth(overclockDamage);
	}
}

