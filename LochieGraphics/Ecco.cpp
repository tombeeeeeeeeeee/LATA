#include "Ecco.h"

#include "Transform.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Collision.h"
#include "SceneObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Sync.h"
#include "Health.h"

#include "EditorGUI.h"
#include "Serialisation.h"

bool Ecco::Update(
	Input::InputDevice& inputDevice, 
	Transform& transform, 
	RigidBody& rigidBody, 
	Health& health,
	float delta, 
	float cameraRotationDelta
)
{

	/*
		check if input is beyond the deadzone
		dot the input with the transform.right()
		desired wheel angle = clamp(asin(the dot product), -maxAngle, maxAngle);
		desired wheel direction = transform.forward() rotated about desired wheel angle
		wheel direction = lerp(wheelDirection, desiredWheeldirection, amountPerSec)

		check if trigger is on
		add force in direction of wheel direction

		if veloctiy exceeds friction coefficent
		spin (add rotational Impulse)

	*/

	//Variables for vector maths
	glm::vec2 force = { 0.0f, 0.0f };
	glm::vec3 right = transform.right();
	glm::vec3 forward = transform.forward();
	glm::vec2 moveInput = inputDevice.getMove();

	//Reading joystick input as wheel direction
	if (glm::length(moveInput) > deadZone)
	{
		moveInput = glm::normalize(moveInput);
		glm::vec2 desiredWheelDirection;
		if (!controlState)
		{
			float c = cosf(cameraRotationDelta * PI / 180.0f);
			float s = sinf(cameraRotationDelta * PI / 180.0f);
			moveInput =
			{
				moveInput.x * c - moveInput.y * s,
				moveInput.x * s + moveInput.y * c
			};

			//Determine angle for wheel change
			float turnAmount = glm::dot(glm::vec2(forward.x, forward.z), moveInput);
			turnAmount = glm::clamp(turnAmount, 0.0f, 1.0f);
			float sign = glm::dot({ right.x, right.z }, moveInput) < 0.0f ? -1.0f : 1.0f;
			float desiredAngle = (glm::acos(turnAmount)) * sign;

			float maxWheelAngleAfterSpeed = maxWheelAngle - speedWheelTurnInfluence / 100.0f * (glm::length(rigidBody.vel)) / (maxCarMoveSpeed)*maxWheelAngle;
			desiredAngle = glm::clamp(desiredAngle, -maxWheelAngleAfterSpeed * PI / 180.0f, maxWheelAngleAfterSpeed * PI / 180.0f);
			//rotate forward by that angle
			c = cosf(desiredAngle);
			s = sinf(desiredAngle);
			desiredWheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };

		}
		else
		{
			float turnAmount = glm::dot({ 1,0 }, moveInput);
			turnAmount = glm::clamp(turnAmount, -1.0f, 1.0f);
			float maxWheelAngleAfterSpeed = maxWheelAngle - speedWheelTurnInfluence / 100.0f * (glm::length(rigidBody.vel)) / (maxCarMoveSpeed)*maxWheelAngle;
			float angle = turnAmount * maxWheelAngleAfterSpeed * PI / 180.0f;
			float c = cosf(angle);
			float s = sinf(angle);
			desiredWheelDirection = { wheelDirection.x * c - wheelDirection.y * s, wheelDirection.y * c + wheelDirection.x * s };
		}
		wheelDirection += (desiredWheelDirection - wheelDirection) * wheelTurnSpeed * delta;
		wheelDirection = glm::normalize(wheelDirection);
	}

	//angleOrWhatever *= pow(0.9, deltaTime * abs(forwardSpeed));

	//Wheel Correction
	float turnSign = glm::sign(glm::dot({ right.x, right.z }, wheelDirection));
	if (glm::dot({ forward.x, forward.z }, wheelDirection) <= glm::cos(maxWheelAngle * PI / 180.0f))
	{
		float maxAngle = maxWheelAngle * PI / 180.0f;

		float c = cosf(turnSign * maxAngle);
		float s = sinf(turnSign * maxAngle);
		wheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };
	}

	timeSinceSpeedBoost += delta;
	if (boosting)
	{
		timeInSpeedBoost += delta;

		force = speedBoost * wheelDirection;
		if (timeInSpeedBoost >= speedBoostDuration)
		{
			boosting = false;
			timeSinceSpeedBoost = 0.0f;
		}
	}
	else
	{
		//Accelerator
		if (glm::length(inputDevice.getRightTrigger()) > 0.01f)
		{
			force += glm::vec2(forward.x, forward.z) * carMoveSpeed * inputDevice.getRightTrigger();
		}

		//Reversaccelrator
		if (glm::length(inputDevice.getLeftTrigger()) > 0.01f)
		{
			force -= glm::vec2(forward.x, forward.z) * carReverseMoveSpeed * inputDevice.getLeftTrigger();
		}

		//Nothingerator
		if (glm::length(force) < 0.001f && glm::length(rigidBody.vel) > 0)
		{
			force += -rigidBody.vel * stoppingFrictionCoef;
		}

		//Stop Speed exceeding speed limit (Could change to be drag)
		if (glm::length(rigidBody.vel + rigidBody.invMass * (force + rigidBody.netForce)) > maxCarMoveSpeed && glm::dot(force, rigidBody.vel) > 0)
		{
			force = glm::normalize(force) * (maxCarMoveSpeed - glm::length(rigidBody.vel)) * 2.0f / rigidBody.invMass;
		}
	}

	float wheelInDirectionOfForward = glm::dot(wheelDirection, { forward.x , forward.z });
	wheelInDirectionOfForward = glm::clamp(wheelInDirectionOfForward, -1.0f, 1.0f);
	//Sideways drag coefficent
	if (glm::length(rigidBody.vel) > 0.00001f)
	{
		float sidewaysMagnitude = glm::dot({ -wheelDirection.y, wheelDirection.x }, rigidBody.vel);
		force += -sidewaysMagnitude * sidewaysFrictionCoef * glm::vec2(-wheelDirection.y, wheelDirection.x);
		force += wheelDirection * sidewaysMagnitude * sidewaysFrictionCoef * (portionOfSidewaysSpeedKept / 100.0f) * (inputDevice.getLeftTrigger() > 0.001f ? -1.0f : 1.0f);
	}

	rigidBody.angularVel = -turningCircleScalar //scalar that represents wheel distance apart
		* acos(wheelInDirectionOfForward) //angle wheel makes with forward vector
		* abs(glm::length(rigidBody.vel)) //units per second
		* glm::sign(glm::dot({ right.x, right.z }, wheelDirection)); //reflects based off of left or right

	//Update rigidBody
	rigidBody.netForce += force;


	if (inputDevice.getButton1())// && lastSpeedBoostPressed + speedBoostCooldown > Time.time)
	{
		if (!boosting && speedBoostUnactuated && timeSinceSpeedBoost >= speedBoostCooldown)
		{
			if (speedBoostInDirectionOfBody)
			{
				wheelDirection = { transform.forward().x, transform.forward().z };
			}

			rigidBody.AddImpulse(speedBoost * wheelDirection);
			health.subtractHealth(speedBoostHPCost);
			boosting = true;
			timeSinceSpeedBoost = 0.0f;
			timeInSpeedBoost = 0.0f;
		}

		speedBoostUnactuated = false;
	}
	else
	{
		speedBoostUnactuated = true;
	}

	timeSinceHealButtonPressed += delta;
	if (inputDevice.getButton2())
	{
		timeSinceHealButtonPressed = 0.0f;
	}
	//TODO add skidding.
	return timeSinceHealButtonPressed <= windowOfTimeForHealPressed;
}

void Ecco::OnCollision(Collision collision)
{
	if (collision.sceneObject->parts & Parts::enemy)
	{
		RigidBody* rb = collision.self->rigidbody();
		if (glm::length(rb->vel) > minSpeedDamageThreshold)
		{
			collision.sceneObject->health()->subtractHealth( speedDamage, collision.sceneObject);
			rb->AddImpulse(glm::normalize(rb->vel) * -speedReductionAfterDamaging);
			collision.self->health()->addHealth(healingFromDamage, collision.self);
			rb->ignoreThisCollision = true;
		}
	}
}

void Ecco::OnHealthDown(HealthPacket healthPacket)
{
	//TODO: Lose Condition.
}

void Ecco::GUI()
{
	//ImGui::Text("");
	if (ImGui::CollapsingHeader("Ecco Component"))
	{
		ImGui::Indent();
		ImGui::DragFloat("Car move speed", &carMoveSpeed);
		ImGui::DragFloat("Car reverse move speed", &carReverseMoveSpeed);
		ImGui::DragFloat("Max car move speed", &maxCarMoveSpeed);
		ImGui::DragFloat("Turning circle scalar", &turningCircleScalar);
		ImGui::DragFloat("Max wheel angle", &maxWheelAngle);
		ImGui::DragFloat("Speed wheel turn influence", &speedWheelTurnInfluence, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("Wheel Turn Speed", &wheelTurnSpeed);
		ImGui::DragFloat("Sideways Wheel Drag", &sidewaysFrictionCoef, 0.01f, 0.0f);
		ImGui::DragFloat("Portion of Sideways Speed Kept", &portionOfSidewaysSpeedKept, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("Stopping Wheel Drag", &stoppingFrictionCoef, 0.01f, 0.0f);
		ImGui::Checkbox("Local Steering", &controlState);
		ImGui::DragFloat("Speed Boost", &speedBoost);
		ImGui::DragInt("Speed boost self damage", &speedBoostHPCost);
		ImGui::DragFloat("Speed Boost Cooldown", &speedBoostCooldown);
		ImGui::DragFloat("Minimum damaging speed", &minSpeedDamageThreshold);
		ImGui::DragInt("On Collision Damage", &speedDamage);
		ImGui::DragInt("On Collision Heal", &healingFromDamage);
		ImGui::DragFloat("Speed reduction after damage", &speedReductionAfterDamaging);
		ImGui::DragInt("Max Health", &maxHealth);
		if (ImGui::DragFloat("Heal Button Tolerance", &windowOfTimeForHealPressed))
		{
			SceneManager::scene->sync->windowOfTimeForHealPressed = windowOfTimeForHealPressed;
		}
		ImGui::BeginDisabled();
		ImGui::DragFloat2(("WheelDirection"), &wheelDirection[0]);
		ImGui::EndDisabled();

		ImGui::Text("");
		ImGui::Text("TESTING PARTS");
		ImGui::Checkbox("Boost Not In Wheel Direction", &speedBoostInDirectionOfBody);
		ImGui::DragFloat("Speed Boost Duration", &speedBoostDuration );
		ImGui::Unindent();
	}
}

toml::table Ecco::Serialise()
{
	return toml::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "wheelDirection", Serialisation::SaveAsVec2(wheelDirection)},
		{ "carMoveSpeed", carMoveSpeed },
		{ "carReverseMoveSpeed", carReverseMoveSpeed},
		{ "maxCarMoveSpeed", maxCarMoveSpeed },
		{ "deadZone", deadZone },
		{ "turningCircleScalar", turningCircleScalar },
		{ "speedWheelTurnInfluence", speedWheelTurnInfluence },
		{ "maxWheelAngle", maxWheelAngle },
		{ "wheelTurnSpeed", wheelTurnSpeed },
		{ "sidewaysFrictionCoef", sidewaysFrictionCoef },
		{ "portionOfSidewaysSpeedKept", portionOfSidewaysSpeedKept },
		{ "stoppingFrictionCoef", stoppingFrictionCoef },
		{ "controlState", controlState},
		{ "speedBoost", speedBoost},
		{ "speedBoostCooldown", speedBoostCooldown},
		{ "minSpeedDamageThreshold", minSpeedDamageThreshold},
		{ "speedDamage", speedDamage},
		{ "healingFromDamage", healingFromDamage},
		{ "speedReductionAfterDamaging", speedReductionAfterDamaging},
		{ "speedBoostDuration", speedBoostDuration},
		{ "maxHealth", maxHealth},
	};
}

Ecco::Ecco(toml::table table)
{
	GUID = Serialisation::LoadAsUnsignedLongLong(table["guid"]);
	wheelDirection = Serialisation::LoadAsVec2(table["wheelDirection"]);
	carMoveSpeed = Serialisation::LoadAsFloat(table["carMoveSpeed"]);
	carReverseMoveSpeed = Serialisation::LoadAsFloat(table["carReverseMoveSpeed"]);
	maxCarMoveSpeed = Serialisation::LoadAsFloat(table["maxCarMoveSpeed"]);
	deadZone = Serialisation::LoadAsFloat(table["deadZone"]);
	turningCircleScalar = Serialisation::LoadAsFloat(table["turningCircleScalar"]);
	speedWheelTurnInfluence = Serialisation::LoadAsFloat(table["speedWheelTurnInfluence"]);
	maxWheelAngle = Serialisation::LoadAsFloat(table["maxWheelAngle"]);
	wheelTurnSpeed = Serialisation::LoadAsFloat(table["wheelTurnSpeed"]);
	sidewaysFrictionCoef = Serialisation::LoadAsFloat(table["sidewaysFrictionCoef"]);
	portionOfSidewaysSpeedKept = Serialisation::LoadAsFloat(table["portionOfSidewaysSpeedKept"]);
	stoppingFrictionCoef = Serialisation::LoadAsFloat(table["stoppingFrictionCoef"]);
	controlState = Serialisation::LoadAsBool(table["controlState"]);
	speedBoost = Serialisation::LoadAsFloat(table["speedBoost"]);
	speedBoostCooldown = Serialisation::LoadAsFloat(table["speedBoostCooldown"]);
	minSpeedDamageThreshold = Serialisation::LoadAsFloat(table["minSpeedDamageThreshold"]);
	speedDamage = Serialisation::LoadAsInt(table["speedDamage"]);
	healingFromDamage = Serialisation::LoadAsInt(table["healingFromDamage"]);
	speedReductionAfterDamaging = Serialisation::LoadAsFloat(table["speedReductionAfterDamaging"]);
	speedBoostDuration = Serialisation::LoadAsFloat(table["speedBoostDuration"]);
	maxHealth = Serialisation::LoadAsInt(table["maxHealth"]);
}


