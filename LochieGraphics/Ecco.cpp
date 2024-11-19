#include "Ecco.h"

#include "Transform.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Collision.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Sync.h"
#include "Health.h"
#include "SceneManager.h"
#include "Directional2dAnimator.h"

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

	Directional2dAnimator* animator = (Directional2dAnimator*)SceneManager::scene->sceneObjects.at(GUID)->animator();

	//Reading joystick input as wheel direction
	if (glm::length(moveInput) > deadZone)
	{
		moveInput = glm::normalize(moveInput);
		glm::vec2 desiredWheelDirection;
		if (!controlState)
		{
			if(inputDevice.getLeftTrigger() && !inputDevice.getRightTrigger())
				cameraRotationDelta += 180.0f;
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
			// TODO: There is just a sign function if want to use that
			float sign = glm::dot({ right.x, right.z }, moveInput) < 0.0f ? -1.0f : 1.0f;
			float desiredAngle = (glm::acos(turnAmount)) * sign;
			float maxWheelAngleAfterSpeed;
			if (glm::dot(rigidBody.vel, rigidBody.vel) > maxCarMoveSpeed * maxCarMoveSpeed)
			{
				maxWheelAngleAfterSpeed = boostWheelTurnInfluence;
			}
			else
			{
				maxWheelAngleAfterSpeed = maxWheelAngle - speedWheelTurnInfluence / 100.0f * (glm::length(rigidBody.vel)) / (maxCarMoveSpeed)*maxWheelAngle;
			}

			desiredAngle = glm::clamp(desiredAngle, -maxWheelAngleAfterSpeed * PI / 180.0f, maxWheelAngleAfterSpeed * PI / 180.0f);
			//rotate forward by that angle
			c = cosf(desiredAngle);
			s = sinf(desiredAngle);
			desiredWheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };

			// TODO: make a get 2d directions like right
			if (animator) {
				glm::vec2 right2D = glm::normalize(glm::vec2(transform.right().x, transform.right().z));
				animator->leftRightLerpAmount = glm::dot(wheelDirection, right2D) * 0.5f + 0.5f;
			}


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
		float maxRelativeSpeed = inputDevice.getLeftTrigger() ? maxReverseSpeed : maxCarMoveSpeed;
		if (glm::length(rigidBody.vel + rigidBody.invMass * (force + rigidBody.netForce)) > maxRelativeSpeed && glm::dot(force, rigidBody.vel) > 0)
		{
			force = glm::normalize(force) * (maxRelativeSpeed - glm::length(rigidBody.vel)) * exceedingSlowIntensity / rigidBody.invMass;
		}
	}

	float wheelInDirectionOfForward = glm::dot(wheelDirection, { forward.x , forward.z });
	wheelInDirectionOfForward = glm::clamp(wheelInDirectionOfForward, -1.0f, 1.0f);
	//Sideways drag coefficent
	if (glm::length(rigidBody.vel) > 0.00001f)
	{
		float sidewaysMagnitude = glm::dot({ wheelDirection.y, -wheelDirection.x }, rigidBody.vel);
		force += -sidewaysMagnitude * sidewaysFrictionCoef * glm::vec2(wheelDirection.y, -wheelDirection.x);
		if(inputDevice.getLeftTrigger())
			force += wheelDirection * abs(sidewaysMagnitude) * sidewaysFrictionCoef * (portionOfSidewaysSpeedKept / 100.0f) * (inputDevice.getLeftTrigger() > 0.001f ? -1.0f : 1.0f);
	}

	float speed = glm::length(rigidBody.vel);
	if (speed > maxCarMoveSpeed)
	{
		rigidBody.angularVel =
			-turningCircleScalarBoosting								//scalar that represents wheel distance apart
			* acos(wheelInDirectionOfForward)							//angle wheel makes with forward vector
			* (glm::min(speed * delta, maxCarMoveSpeed))				//units per second
			* glm::sign(glm::dot({ right.x, right.z }, wheelDirection));//reflects based off of left or right
	}
	else
	{
		rigidBody.angularVel = 
			-turningCircleScalar										//scalar that represents wheel distance apart
			* acos(wheelInDirectionOfForward)							//angle wheel makes with forward vector
			* (glm::min (speed * delta, maxCarMoveSpeed))				//units per second
			* glm::sign(glm::dot({ right.x, right.z }, wheelDirection));//reflects based off of left or right
	}

	//Update rigidBody
	rigidBody.netForce += force;


	if (inputDevice.getButton1())// && lastSpeedBoostPressed + speedBoostCooldown > Time.time)
	{
		if (health.currHealth > speedBoostHPCost)
		{
			if (!boosting && speedBoostUnactuated && timeSinceSpeedBoost >= speedBoostCooldown)
			{
				if (speedBoostInDirectionOfBody)
				{
					wheelDirection = { forward.x, forward.z };
				}

				rigidBody.AddImpulse(speedBoost * wheelDirection);
				health.subtractHealth(speedBoostHPCost);
				boosting = true;
				timeSinceSpeedBoost = 0.0f;
				timeInSpeedBoost = 0.0f;
				SceneManager::scene->audio.PlaySound(Audio::eccoBoost);
				Particle* boost = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, transform.getGlobalPosition());
				boost->explodeStrength = 5.0f;
				boost->Explode();
				boost->Explode();
				boost->Explode();
			}
		}

		speedBoostUnactuated = false;
	}
	else
	{
		speedBoostUnactuated = true;
	}

	timeSinceLastHeal += delta;
	timeSinceHealButtonPressed += delta;
	if (inputDevice.getButton2())
	{
		timeSinceHealButtonPressed = 0.0f;
	}
	//TODO add skidding.

	if (animator) {
		glm::vec2 forward = glm::normalize(glm::vec2{ transform.forward().x, transform.forward().z });
		float forwardVelocity = glm::dot(rigidBody.vel, forward);
		if (forwardVelocity <= 0) {
			float backwardsPercent = -forwardVelocity / maxReverseSpeed;
			animator->downUpLerpAmount = -backwardsPercent / 2 + 0.5f;
		}
		else {
			animator->downUpLerpAmount = glm::clamp((forwardVelocity / (maxCarMoveSpeed * 1.5f)) / 2 + 0.5f, 0.5f, 1.0f);
		}
	}

	return timeSinceHealButtonPressed <= windowOfTimeForHealPressed;
}

void Ecco::OnCollision(Collision collision)
{
	if (collision.sceneObject->parts & Parts::enemy)
	{
		RigidBody* rb = collision.self->rigidbody();
		if (glm::dot(rb->vel, collision.normal) > 0.2f)
		{
			float speed = glm::length(rb->vel);
			if (speed > maxCarMoveSpeed)
			{
				if (collision.sceneObject->health()->subtractHealth(boostDamage))
				{
					collision.rigidBody->AddImpulse(collision.normal * boostKnockback);
					SceneManager::scene->audio.PlaySound(Audio::eccoEnemyHit);
					if(timeSinceLastHeal <= healCooldown)
					{
						collision.self->health()->addHealth(healingFromDamage);
						timeSinceLastHeal = 0.0f;
					}
					collision.self->rigidbody()->AddImpulse(collision.normal * -speedReductionAfterDamaging);
				}
			}
			else if (glm::length(rb->vel) > minSpeedDamageThreshold)
			{
				if (collision.sceneObject->health()->subtractHealth(speedDamage))
				{
					collision.rigidBody->AddImpulse(collision.normal * speedKnockback);
					SceneManager::scene->audio.PlaySound(Audio::eccoEnemyHit);
					if (timeSinceLastHeal <= healCooldown)
					{
						collision.self->health()->addHealth(healingFromDamage);
						timeSinceLastHeal = 0.0f;
					}
					collision.self->rigidbody()->AddImpulse(collision.normal * -speedReductionAfterDamaging);
				}
			}
		}
	}
}

void Ecco::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	//ImGui::Text("");
	if (ImGui::CollapsingHeader(("Ecco Component##" + tag).c_str()))
	{
		ImGui::Indent();
		ImGui::DragFloat(("Car move speed##" + tag).c_str(), &carMoveSpeed);
		ImGui::DragFloat(("Car reverse move speed##" + tag).c_str(), &carReverseMoveSpeed);
		ImGui::DragFloat(("Max car move speed##" + tag).c_str(), &maxCarMoveSpeed);
		ImGui::DragFloat(("Max reverse speed##" + tag).c_str(), &maxReverseSpeed);
		ImGui::DragFloat(("Turning circle scalar##" + tag).c_str(), &turningCircleScalar);
		ImGui::DragFloat(("Boosting turning circle scalar##" + tag).c_str(), &turningCircleScalarBoosting);
		ImGui::DragFloat(("Max wheel angle##" + tag).c_str(), &maxWheelAngle);
		ImGui::DragFloat(("Speed wheel turn influence##" + tag).c_str(), &speedWheelTurnInfluence, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat(("boost wheel turn influence##" + tag).c_str(), &boostWheelTurnInfluence, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat(("Wheel Turn Speed##" + tag).c_str(), &wheelTurnSpeed);
		ImGui::DragFloat(("Sideways Wheel Drag##" + tag).c_str(), &sidewaysFrictionCoef, 0.01f, 0.0f);
		ImGui::DragFloat(("Portion of Sideways Speed Kept##" + tag).c_str(), &portionOfSidewaysSpeedKept, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat(("Stopping Wheel Drag##" + tag).c_str(), &stoppingFrictionCoef, 0.01f, 0.0f);
		ImGui::DragFloat(("Exceeding Max Slowing Force##" + tag).c_str(), &exceedingSlowIntensity, 0.1f, 0.0f);
		ImGui::Checkbox(("Local Steering##" + tag).c_str(), &controlState);
		ImGui::DragFloat(("Speed Boost##" + tag).c_str(), &speedBoost);
		ImGui::DragInt(("Speed boost self damage##" + tag).c_str(), &speedBoostHPCost);
		ImGui::DragFloat(("Speed Boost Cooldown##" + tag).c_str(), &speedBoostCooldown);
		ImGui::DragFloat(("Minimum damaging speed##" + tag).c_str(), &minSpeedDamageThreshold);
		ImGui::DragInt(("On Collision Damage##" + tag).c_str(), &speedDamage);
		ImGui::DragInt(("On Collision Damage BOOSTING##" + tag).c_str(), &boostDamage);
		ImGui::DragInt(("On Collision Heal##" + tag).c_str(), &healingFromDamage);
		ImGui::DragFloat(("Siphon Cooldown##" + tag).c_str(), &healCooldown);
		ImGui::DragFloat(("Speeding Knock Back##" + tag).c_str(), &speedKnockback);
		ImGui::DragFloat(("Boosting Knock Back##" + tag).c_str(), &boostKnockback);
		ImGui::DragFloat(("Speed reduction after damage##" + tag).c_str(), &speedReductionAfterDamaging);
		ImGui::DragInt(("Max Health##" + tag).c_str(), &maxHealth);
		if (ImGui::DragFloat(("Heal Button Tolerance##" + tag).c_str(), &windowOfTimeForHealPressed))
		{
			SceneManager::scene->sync->windowOfTimeForHealPressed = windowOfTimeForHealPressed;
		}
		ImGui::BeginDisabled();
		ImGui::DragFloat2((("WheelDirection##" + tag).c_str()), &wheelDirection[0]);
		ImGui::EndDisabled();
		ImGui::Checkbox(("Boost Not In Wheel Direction##" + tag).c_str(), &speedBoostInDirectionOfBody);
		ImGui::DragFloat(("Speed Boost Duration##" + tag).c_str(), &speedBoostDuration);

		if (ImGui::CollapsingHeader(("Health UI##" + tag).c_str())) {
			healthUI.GUI();
		}
		if (ImGui::CollapsingHeader(("Boost UI##" + tag).c_str())) {
			boostUI.GUI();
		}

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
		{ "maxReverseSpeed", maxReverseSpeed },
		{ "deadZone", deadZone },
		{ "turningCircleScalar", turningCircleScalar },
		{ "turningCircleScalarBoosting", turningCircleScalarBoosting },
		{ "speedWheelTurnInfluence", speedWheelTurnInfluence },
		{ "boostWheelTurnInfluence", boostWheelTurnInfluence },
		{ "maxWheelAngle", maxWheelAngle },
		{ "wheelTurnSpeed", wheelTurnSpeed },
		{ "sidewaysFrictionCoef", sidewaysFrictionCoef },
		{ "portionOfSidewaysSpeedKept", portionOfSidewaysSpeedKept },
		{ "stoppingFrictionCoef", stoppingFrictionCoef },
		{ "exceedingSlowIntensity", exceedingSlowIntensity },
		{ "controlState", controlState},
		{ "speedBoost", speedBoost},
		{ "speedBoostHPCost", speedBoostHPCost },
		{ "speedBoostCooldown", speedBoostCooldown},
		{ "minSpeedDamageThreshold", minSpeedDamageThreshold},
		{ "speedDamage", speedDamage},
		{ "healingFromDamage", healingFromDamage},
		{ "speedReductionAfterDamaging", speedReductionAfterDamaging},
		{ "speedBoostDuration", speedBoostDuration},
		{ "maxHealth", maxHealth},
		{ "speedBoostInDirectionOfBody", speedBoostInDirectionOfBody },
		{ "healthUI", healthUI.Serialise() },
		{ "boostUI", boostUI.Serialise() },
		{ "boostDamage", boostDamage },
		{ "boostKnockback", boostKnockback },
		{ "speedKnockback", speedKnockback },
		{ "healCooldown", healCooldown },
	};
}

float Ecco::getSpeedBoostCooldownPercent() const
{
	return timeSinceSpeedBoost / speedBoostCooldown;
}

Ecco::Ecco(toml::table table)
{
	GUID = Serialisation::LoadAsUnsignedLongLong(table["guid"]);
	wheelDirection = Serialisation::LoadAsVec2(table["wheelDirection"]);
	carMoveSpeed = Serialisation::LoadAsFloat(table["carMoveSpeed"]);
	carReverseMoveSpeed = Serialisation::LoadAsFloat(table["carReverseMoveSpeed"]);
	maxCarMoveSpeed = Serialisation::LoadAsFloat(table["maxCarMoveSpeed"]);
	maxReverseSpeed = Serialisation::LoadAsFloat(table["maxReverseSpeed"]);
	deadZone = Serialisation::LoadAsFloat(table["deadZone"]);
	turningCircleScalar = Serialisation::LoadAsFloat(table["turningCircleScalar"]);
	turningCircleScalarBoosting = Serialisation::LoadAsFloat(table["turningCircleScalarBoosting"], 0.06f);
	speedWheelTurnInfluence = Serialisation::LoadAsFloat(table["speedWheelTurnInfluence"]);
	boostWheelTurnInfluence = Serialisation::LoadAsFloat(table["boostWheelTurnInfluence"]);
	maxWheelAngle = Serialisation::LoadAsFloat(table["maxWheelAngle"]);
	wheelTurnSpeed = Serialisation::LoadAsFloat(table["wheelTurnSpeed"]);
	sidewaysFrictionCoef = Serialisation::LoadAsFloat(table["sidewaysFrictionCoef"]);
	portionOfSidewaysSpeedKept = Serialisation::LoadAsFloat(table["portionOfSidewaysSpeedKept"]);
	stoppingFrictionCoef = Serialisation::LoadAsFloat(table["stoppingFrictionCoef"]);
	exceedingSlowIntensity = Serialisation::LoadAsFloat(table["exceedingSlowIntensity"]);
	controlState = Serialisation::LoadAsBool(table["controlState"]);
	speedBoost = Serialisation::LoadAsFloat(table["speedBoost"]);
	speedBoostHPCost = Serialisation::LoadAsInt(table["speedBoostHPCost"]);
	speedBoostCooldown = Serialisation::LoadAsFloat(table["speedBoostCooldown"]);
	minSpeedDamageThreshold = Serialisation::LoadAsFloat(table["minSpeedDamageThreshold"]);
	speedDamage = Serialisation::LoadAsInt(table["speedDamage"]);
	healingFromDamage = Serialisation::LoadAsInt(table["healingFromDamage"]);
	speedReductionAfterDamaging = Serialisation::LoadAsFloat(table["speedReductionAfterDamaging"]);
	speedBoostDuration = Serialisation::LoadAsFloat(table["speedBoostDuration"]);
	maxHealth = Serialisation::LoadAsInt(table["maxHealth"]);
	speedBoostInDirectionOfBody = Serialisation::LoadAsInt(table["speedBoostInDirectionOfBody"]);
	boostDamage = Serialisation::LoadAsInt(table["boostDamage"]);
	speedKnockback = Serialisation::LoadAsFloat(table["speedKnockback"], 10.0f);
	boostKnockback = Serialisation::LoadAsFloat(table["boostKnockback"], 10.0f);
	healCooldown = Serialisation::LoadAsFloat(table["healCooldown"], 1.0f);

	healthUI.Load(table["healthUI"].as_table());
	boostUI.Load(table["boostUI"].as_table());
}


