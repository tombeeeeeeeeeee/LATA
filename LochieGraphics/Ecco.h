#pragma once

#include "Utilities.h"

#include "Input.h"

#include "Serialisation.h"

class Transform;
class RigidBody;


class Ecco
{
public:
	unsigned long long GUID = 0;


	Ecco() {};
	Ecco(toml::table table);

	void Update(
		Input::InputDevice& inputDevice,
		Transform& transform,
		RigidBody& rigidBody,
		float delta,
		float cameraRotationDelta
	);

	void GUI();
	toml::table Serialise();

	glm::vec2 wheelDirection = { 0.0f, 0.0f };

private:
	float carMoveSpeed = 0.25f;
	float carReverseMoveSpeed = 0.125f;
	float maxCarMoveSpeed = 6.0f;
	float deadZone = 0.6f;
	float turningCircleScalar = 6.0f;
	float speedWheelTurnInfluence = 50.0f;
	float maxWheelAngle = 60.0f;
	float wheelTurnSpeed = 4.0f;
	float sidewaysFrictionCoef = 0.65f;
	float portionOfSidewaysSpeedKept = 100.0f;
	float stoppingFrictionCoef = 0.8f;
	bool controlState = false;
};