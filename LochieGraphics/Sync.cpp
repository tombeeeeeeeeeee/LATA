#include "Sync.h"

#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"

#include "EditorGUI.h"

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
		//Begin Chagrging Shot
		if (!chargingShot)
		{
			chargingShot = true;
			chargedDuration = 0.0f;
		}

		//Charging shot.
		if (chargedDuration < sniperChargeTime && currCharge > sniperChargeCost)
		{
			if (chargedDuration + delta > sniperChargeTime)
			{
				//Do charging stuff
			}
			chargedDuration += delta;
		}
		else if (chargedDuration < overclockChargeTime && currCharge > overclockChargeCost)
		{
			if (chargedDuration + delta > overclockChargeTime) 
			{
			//Do charging stuff
			}
			chargedDuration += delta;
		}
	
		//TODO: add rumble
	}
	else if (chargingShot)
	{
		chargingShot = false;
		if (chargedDuration >= overclockChargeTime)
		{
			ShootOverClocked();
		}
		else if (chargedDuration >= sniperChargeTime)
		{
			ShootSniper();
		}
		else
		{
			ShootMisfire();
		}
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
		ImGui::DragFloat("Misfire Charge Cost", &misfireChargeCost);
		ImGui::DragFloat("Sniper Damage", &sniperDamage);
		ImGui::DragFloat("Sniper Charge Cost", &sniperChargeCost);
		ImGui::DragFloat("Sniper Charge Time", &sniperChargeTime);
		ImGui::DragFloat("Overclock Damage", &overclockDamage);
		ImGui::DragFloat("Overclock Charge Cost", &overclockChargeCost);
		ImGui::DragFloat("Overlock Charge Time", &overclockChargeTime);

	}
}

void Sync::ShootMisfire()
{

}

void Sync::ShootSniper()
{
}

void Sync::ShootOverClocked()
{
}

void Sync::ShootOverClockedSplit(glm::vec3 dir, int num)
{
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

