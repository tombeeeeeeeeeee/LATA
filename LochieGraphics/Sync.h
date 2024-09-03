#pragma once
#include "Input.h"

#include "Maths.h"

class Transform;
class RigidBody;
struct Collision;

class Sync
{
public:
	unsigned long long GUID = 0;


	Sync() {};

	void Update(
		Input::InputDevice& inputDevice,
		Transform& transform,
		RigidBody& rigidBody,
		float delta	
	);

	void GUI();

	void ShootMisfire();
	void ShootSniper();
	void ShootOverClocked();
	void ShootOverClockedSplit(glm::vec3 dir, int num);

	void misfireShotOnCollision(Collision collision);
	void sniperShotOnCollision(Collision collision);
	void overclockShotOnCollision(Collision collision);

private:

	std::vector<unsigned long long> misfireShots;
	std::vector<unsigned long long> sniperShots;
	std::vector<unsigned long long> overclockShots;

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;

	float misfireDamage = 0.0f;
	float misfireChargeCost = 0.0f;
	float sniperDamage = 0.0f;
	float sniperChargeCost = 0.0f;
	float overclockDamage = 0.0f;
	float overclockChargeCost = 0.0f;

	float sniperChargeTime = 0.0f;
	float overclockChargeTime = 0.0f;

	float chargedDuration = 0.0f;
	bool chargingShot = false;
	float currCharge = 0.0f;
	float maxCharge = 10.0f;
};
