#pragma once

#include "Utilities.h"

#include "Input.h"

#include "Serialisation.h"

class Transform;
class RigidBody;
class Health;
struct HealthPacket;
struct Collision;

class Ecco
{
public:
	unsigned long long GUID = 0;


	Ecco() {};
	Ecco(toml::table table);
	void Start(
		Health& health
	);
	void Update(
		Input::InputDevice& inputDevice,
		Transform& transform,
		RigidBody& rigidBody,
		Health& health,
		float delta,
		float cameraRotationDelta
	);

	void OnCollision(Collision collision);
	void OnHealthDown(HealthPacket healthPacket);

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
	float speedBoost = 1.0f;
	float speedBoostCooldown = 0.1f;
	int speedBoostHPCost = 0;
	bool speedBoostUnactuated = true;
	float lastSpeedBoostPressed = 0.0f;
	float minSpeedDamageThreshold = 0.0f;
	int speedDamage = 0;
	int healingFromDamage = 0;
	float speedReductionAfterDamaging = 0.0f;
	int maxHealth = 10;
};