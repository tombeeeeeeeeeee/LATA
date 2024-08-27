#pragma once
#include "Input.h"

#include "Maths.h"

class Transform;
class RigidBody;

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

private:
	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;
};
