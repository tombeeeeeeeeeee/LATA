#include "Ecco.h"
#include "imgui.h"

void Ecco::Update(Input::InputDevice& inputDevice, Transform& transform, RigidBody& rigidBody, float delta)
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
			//Determine angle for wheel change
			float turnAmount = glm::dot(glm::vec2(forward.x, forward.z), moveInput);
			turnAmount = glm::clamp(turnAmount, 0.0f, 1.0f);
			float sign = glm::dot({ right.x, right.z }, moveInput) < 0.0f ? -1.0f : 1.0f;
			float desiredAngle = (glm::acos(turnAmount)) * sign + PI / 4;

			desiredAngle = glm::clamp(desiredAngle, -maxWheelAngle * PI / 180.0f, maxWheelAngle * PI / 180.0f);
			//rotate forward by that angle
			float c = cosf(desiredAngle);
			float s = sinf(desiredAngle);
			desiredWheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };

		}
		else
		{
			float turnAmount = glm::dot({1,0}, moveInput);
			turnAmount = glm::clamp(turnAmount, -1.0f, 1.0f);

			float angle = turnAmount * maxWheelAngle * PI / 180.0f;
			float c = cosf(angle);
			float s = sinf(angle);
			desiredWheelDirection = { wheelDirection.x * c - wheelDirection.y * s, wheelDirection.y * c + wheelDirection.x * s };
		}
		wheelDirection += (desiredWheelDirection - wheelDirection) * wheelTurnSpeed * delta;
		wheelDirection = glm::normalize(wheelDirection);
	}

	//Wheel Correction
	if (glm::dot({ forward.x, forward.z }, wheelDirection) <= glm::cos(maxWheelAngle * PI / 180.0f))
	{
		float turnSign = glm::sign(glm::dot({ right.x, right.z }, wheelDirection));
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

	//Sideways drag coefficent
	if (glm::length(rigidBody.vel) > 0.00001f)
	{
		float sidewaysForceCoef = glm::dot({ wheelDirection.y, -wheelDirection.x }, rigidBody.vel);
		force += -glm::abs(sidewaysForceCoef * sidewaysForceCoef) * sidewaysFrictionCoef * glm::normalize(rigidBody.vel);
	}

	//Stop Speed exceeding speed limit (Could change to be drag)
	if (glm::length(rigidBody.vel + rigidBody.invMass * (force + rigidBody.netForce)) > maxCarMoveSpeed)
	{
		force = glm::normalize(force) * (maxCarMoveSpeed - glm::length(rigidBody.vel)) / rigidBody.invMass;
	}

	//Update rigidBody
	rigidBody.netForce += force;
	rigidBody.angularVel += -turningCircleScalar * glm::length(rigidBody.vel) * glm::dot({ right.x, right.z }, wheelDirection) * glm::sign(glm::dot(rigidBody.vel, {forward.x, forward.z}));
	rigidBody.angularVel *= 0.85f;

	//TODO add skidding.
}

void Ecco::GUI()
{
	ImGui::DragFloat("Car move speed", &carMoveSpeed);
	ImGui::DragFloat("Car reverse move speed", &carReverseMoveSpeed);
	ImGui::DragFloat("Max car move speed", &maxCarMoveSpeed);
	ImGui::DragFloat("Turning circle scalar", &turningCircleScalar);
	ImGui::DragFloat("Max wheel angle", &maxWheelAngle);
	ImGui::DragFloat("Wheel Turn Speed", &wheelTurnSpeed);
	ImGui::DragFloat("Sideways Wheel Drag", &sidewaysFrictionCoef, 0.01f, 0.0f);
	ImGui::DragFloat("Stopping Wheel Drag", &stoppingFrictionCoef, 0.01f, 0.0f);
	ImGui::Checkbox("Local Steering", &controlState);

	ImGui::BeginDisabled();
	ImGui::DragFloat2(("WheelDirection"), &wheelDirection[0]);
	ImGui::EndDisabled();
}
