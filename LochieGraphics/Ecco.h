#pragma once

#include "Transform.h"
#include "Rigidbody.h"
#include "Input.h"

#include "math.h"

class Ecco
{
public:
	unsigned long long GUID = 0;


	Ecco() {};

	void Update(
		Input::InputDevice& inputDevice,
		Transform& transform,
		RigidBody& rigidBody,
		float delta
	);

	void GUI();

	glm::vec2 wheelDirection = { 0.0f, 0.0f };

private:
	float carMoveSpeed = 0.25f;
	float maxCarMoveSpeed = 6.0f;
	float deadZone = 0.6f;
	float turningCircleScalar = 1.5f;
	float maxWheelAngle = 0.9f;
	float wheelTurnSpeed = 4.0f;
	float sidewaysFrictionCoef = 0.3f;
	float stoppingFrictionCoef = 0.3f;
};