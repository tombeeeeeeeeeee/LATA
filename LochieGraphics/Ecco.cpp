#include "Ecco.h"

#include "Transform.h"
#include "RigidBody.h"

#include "EditorGUI.h"

void Ecco::Update(Input::InputDevice& inputDevice, Transform& transform, RigidBody& rigidBody, float delta, float cameraRotationDelta)
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
	glm::vec2 force = {0.0f, 0.0f};
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

			float maxWheelAngleAfterSpeed = maxWheelAngle - speedWheelTurnInfluence/100.0f * (glm::length(rigidBody.vel)) / (maxCarMoveSpeed)*maxWheelAngle;
			desiredAngle = glm::clamp(desiredAngle, -maxWheelAngleAfterSpeed * PI / 180.0f, maxWheelAngleAfterSpeed * PI / 180.0f);
			//rotate forward by that angle
			c = cosf(desiredAngle);
			s = sinf(desiredAngle);
			desiredWheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };

		}
		else
		{
			float turnAmount = glm::dot({1,0}, moveInput);
			turnAmount = glm::clamp(turnAmount, -1.0f, 1.0f);
			float maxWheelAngleAfterSpeed = maxWheelAngle - speedWheelTurnInfluence/100.0f * (glm::length(rigidBody.vel)) / (maxCarMoveSpeed)*maxWheelAngle;
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
	if(glm::length(force) < 0.001f && glm::length(rigidBody.vel) > 0)
	{
		force += -rigidBody.vel * glm::length(rigidBody.vel) * stoppingFrictionCoef;
	}

	//Stop Speed exceeding speed limit (Could change to be drag)
	if (glm::length(rigidBody.vel + rigidBody.invMass * (force + rigidBody.netForce)) > maxCarMoveSpeed)
	{
		force = glm::normalize(force) * (maxCarMoveSpeed - glm::length(rigidBody.vel)) / rigidBody.invMass;
	}

	float wheelInDirectionOfForward = glm::dot(wheelDirection, { forward.x , forward.z });
	wheelInDirectionOfForward = glm::clamp(wheelInDirectionOfForward, -1.0f, 1.0f);

	//Sideways drag coefficent
	if (glm::length(rigidBody.vel) > 0.00001f)
	{
		float sidewaysForceCoef = glm::dot({ wheelDirection.y, -wheelDirection.x }, glm::normalize(rigidBody.vel));
		force += -glm::abs(sidewaysForceCoef * sidewaysForceCoef) * sidewaysFrictionCoef * rigidBody.vel;
		force += wheelDirection * glm::abs(sidewaysForceCoef * sidewaysForceCoef) * sidewaysFrictionCoef * glm::length(rigidBody.vel) * (portionOfSidewaysSpeedKept / 100.0f) * (inputDevice.getLeftTrigger() > 0.001f ? -1.0f : 1.0f);
	}

	rigidBody.angularVel = -turningCircleScalar //scalar that represents wheel distance apart
		* acos(wheelInDirectionOfForward) //angle wheel makes with forward vector
		* abs(glm::length(rigidBody.vel)) //units per second
		* glm::sign(glm::dot({ right.x, right.z }, wheelDirection)); //reflects based off of left or right

	//Update rigidBody
	rigidBody.netForce += force;

	//TODO add skidding.
}

void Ecco::GUI()
{
	//ImGui::Text("");
	if(ImGui::CollapsingHeader("Ecco Component"))
	{
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

		ImGui::BeginDisabled();
		ImGui::DragFloat2(("WheelDirection"), &wheelDirection[0]);
		ImGui::EndDisabled();
	}
}
