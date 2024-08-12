#include "Sync.h"
#include "imgui.h"

void Sync::Update(Input::InputDevice& inputDevice, Transform& transform, RigidBody& rigidBody, float delta)
{
	glm::vec2 look = inputDevice.getLook();
	glm::vec2 move = inputDevice.getMove();
	if (glm::length(look) > lookDeadZone)
	{
		float turnAmount = glm::dot(transform.forward(), { look.x, 0.0f, look.y });
		float desiredAngle = glm::acos(turnAmount);
		glm::vec3 eulers = transform.getEulerRotation();
		transform.setEulerRotation({ eulers.x, eulers.y + desiredAngle, eulers.z });
	}
	if (glm::length(move) > moveDeadZone)
		rigidBody.vel = move;
	else
		rigidBody.vel = { 0.0f, 0.0f };
}

void Sync::GUI()
{
	ImGui::DragFloat("Look DeadZone", &lookDeadZone);
	ImGui::DragFloat("Move DeadZone", &moveDeadZone);
}
