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
	//void meidumShotOnCollision(Collision collision);
	//void bigShotOnCollision(Collision collision);

private:

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;


};
