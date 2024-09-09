#pragma once
#include "Input.h"

#include "Maths.h"
#include "Serialisation.h"

class Transform;
class RigidBody;
struct Collision;

class Sync
{
public:
	unsigned long long GUID = 0;


	Sync() {};
	Sync(toml::table table);

	void Update(
		Input::InputDevice& inputDevice,
		Transform& transform,
		RigidBody& rigidBody,
		float delta	
	);

	void GUI();

	toml::table Serialise() const;

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
	float sniperDamage = 0.0f;
	float overclockDamage = 0.0f;

	float sniperChargeTime = 0.0f;
	float overclockChargeTime = 0.0f;

	float timeAtStartShooting = 0.0f;
	bool chargingShot = false;
};
