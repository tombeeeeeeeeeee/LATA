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

	void smallShotOnCollision(Collision collision);
	void meidumShotOnCollision(Collision collision);
	void bigShotOnCollision(Collision collision);

private:

	std::vector<unsigned long long> smallShots;
	std::vector<unsigned long long> mediumShots;
	std::vector<unsigned long long> bigShots;

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;

	float smallDamage = 0.0f;
	float mediumDamage = 0.0f;
	float bigDamage = 0.0f;
};
