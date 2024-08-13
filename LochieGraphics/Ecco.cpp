#include "Ecco.h"
#include "imgui.h"

void Ecco::Update(Input::InputDevice& inputDevice, Transform& transform, RigidBody& rigidBody, float delta)
{

	/*
	* check if input is beyond the deadzone
	* dot the input with the transform.right()
	* desired wheel angle = clamp(asin(the dot product), -maxAngle, maxAngle);
	* desired wheel direction = transform.forward() rotated about desired wheel angle
	* wheel direction = lerp(wheelDirection, desiredWheeldirection, amountPerSec)
	*
	* check if trigger is on
	* add force in direction of wheel direction
	*
	* if veloctiy exceeds friction coefficent
	* spin (add rotational Impulse)
	*
	*/

	//Variables for vectior maths
	glm::vec2 force = {0.0f, 0.0f};
	glm::vec3 right = transform.right();
	glm::vec3 forward = transform.forward();
	glm::vec2 moveInput = inputDevice.getMove();

	//Reading joystick input as wheel direction
	if (glm::length(moveInput) > deadZone)
	{
		moveInput = glm::normalize(moveInput);
		
		//Determine angle for wheel change
		float turnAmount = glm::dot(glm::vec2(right.x, right.z), moveInput);
		turnAmount = glm::clamp(turnAmount, -1.0f, 1.0f);
		float desiredAngle = glm::asin(turnAmount);
		desiredAngle = glm::clamp(desiredAngle, -maxWheelAngle, maxWheelAngle);

		//rotate forward by that angle
		float c = cosf(desiredAngle);
		float s = sinf(desiredAngle);
		glm::vec2 desiredWheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };
		wheelDirection += (desiredWheelDirection - wheelDirection) * wheelTurnSpeed * delta;
		wheelDirection = glm::normalize(wheelDirection);
	}

	//Accelerator
	if (glm::length(inputDevice.getRightTrigger()) > 0.01f)
	{
		force += glm::vec2(forward.x, forward.z) * carMoveSpeed * inputDevice.getRightTrigger();
	}

	//Reversaccelrator
	if (glm::length(inputDevice.getLeftTrigger()) > 0.01f)
	{
		force -= glm::vec2(forward.x, forward.z) * carMoveSpeed * inputDevice.getLeftTrigger();
	}

	//Nothingerator
	if(glm::length(force) < 0.001f)
	{
		force += -rigidBody.vel * stoppingFrictionCoef;
	}

	//Sideways drag coefficent
	float sidewaysForceCoef = glm::dot({ wheelDirection.y, -wheelDirection.x }, rigidBody.vel);
	force += -glm::abs(0.1f * sidewaysForceCoef * sidewaysForceCoef) * sidewaysFrictionCoef * rigidBody.vel;

	//Stop Speed exceeding speed limit (Could change to be drag)
	if (glm::length(rigidBody.vel + rigidBody.invMass * (force + rigidBody.netForce)) > maxCarMoveSpeed)
	{
		force = glm::normalize(force) * (maxCarMoveSpeed - glm::length(rigidBody.vel)) / rigidBody.invMass;
	}

	//Update rigidBody
	rigidBody.netForce += force;
	rigidBody.angularVel = -turningCircleScalar * glm::length(rigidBody.vel) * glm::dot({ right.x, right.z }, wheelDirection) * glm::sign(glm::dot(rigidBody.vel, {forward.x, forward.z}));

	//TODO add skidding.


	//Reversing wheel correction
	if (glm::dot(wheelDirection, { forward.x, forward.z }) >= maxWheelAngle)
	{
		float turnAmount = glm::dot(glm::vec2(right.x, right.z), wheelDirection);
		turnAmount = glm::clamp(turnAmount, -1.0f, 1.0f);
		float desiredAngle = glm::asin(turnAmount);
		desiredAngle = glm::clamp(desiredAngle, -maxWheelAngle, maxWheelAngle);

		float c = cosf(desiredAngle);
		float s = sinf(desiredAngle);
		wheelDirection = { forward.x * c - forward.z * s, forward.z * c + forward.x * s };
	}
}

void Ecco::GUI()
{
	ImGui::DragFloat("Car move speed", &carMoveSpeed);
	ImGui::DragFloat("Max car move speed", &maxCarMoveSpeed);
	ImGui::DragFloat("Turning circle scalar", &turningCircleScalar);
	ImGui::DragFloat("Max wheel angle", &maxWheelAngle);
	ImGui::DragFloat("Wheel Turn Speed", &wheelTurnSpeed);
	ImGui::DragFloat("Sideways Wheel Drag", &sidewaysFrictionCoef, 0.01f, 0.0f);
	ImGui::DragFloat("Stopping Wheel Drag", &stoppingFrictionCoef, 0.01f, 0.0f);


	ImGui::BeginDisabled();
	ImGui::DragFloat2(("WheelDirection"), &wheelDirection[0]);
	ImGui::EndDisabled();
}
