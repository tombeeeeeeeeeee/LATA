#include "Input.h"

#include "SceneManager.h"

#include "Utilities.h"

#include "Graphics.h"

#include <iostream>

Input* Input::input = nullptr;

void Input::Initialise()
{
	input = this;
	glfwSetJoystickCallback(JoystickCallback);

	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		if (glfwJoystickPresent(i)) {
			if (glfwJoystickIsGamepad(i)) {
				AddGamepad(i);
			}
			else {
				// Unrecognised controller
			}
		}
	}
}

void Input::JoystickCallback(int id, int event)
{
	input->JoystickChange(id, event);
}

void Input::ShowAllControllerSlotStatuses()
{
	std::cout << "JOYSTICK STATUSES\n";
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		int present = glfwJoystickPresent(i);
		if (present == GLFW_TRUE) {
			std::cout << "Joystick #" << i << " is connected\n";
			std::cout << "\tJoystick name: " << glfwGetJoystickName(i) << "\n";
			if (glfwJoystickIsGamepad(i)) {
				std::cout << "\tRecognised as a gamepad\n";
				std::cout << "\tGamepad name: " << glfwGetGamepadName(i) << "\n";
			}
		}
	}
}

void Input::Update()
{
	for (InputDevice* i : inputDevices)
	{
		if (i->getType() != Type::Controller) {
			continue;
		}
		Controller* c = (Controller*)i;
		c->previousState = c->currentState;
		glfwGetGamepadState(c->id, &c->currentState);
	}
}

void Input::GUI()
{
	if (ImGui::Begin("Input Debug")) {
		if (ImGui::Button("Show all controller connection information (console)")) {
			ShowAllControllerSlotStatuses();
		}
		for (int i = 0; i < inputDevices.size(); i++)
		{
			if (ImGui::CollapsingHeader(("#" + std::to_string(i)).c_str())) {
				glm::vec2 move = inputDevices[i]->getMove();
				glm::vec2 look = inputDevices[i]->getLook();
				float leftTrigger = inputDevices[i]->getLeftTrigger();
				float rightTrigger = inputDevices[i]->getRightTrigger();

				ImGui::BeginDisabled();

				ImGui::DragFloat2(("Move##" + Utilities::PointerToString(inputDevices[i])).c_str(), &(move.x));
				ImGui::DragFloat2(("Look##" + Utilities::PointerToString(inputDevices[i])).c_str(), &(look.x));
				ImGui::DragFloat(("Left Trigger##" + Utilities::PointerToString(inputDevices[i])).c_str(), &leftTrigger);
				ImGui::DragFloat(("Right Trigger##" + Utilities::PointerToString(inputDevices[i])).c_str(), &rightTrigger);
				
				ImGui::EndDisabled();
			}
		}
	}
	ImGui::End();
}

void Input::AddGamepad(int id)
{
	inputDevices.push_back(new Controller(id));
}

void Input::AddKeyboard()
{
	inputDevices.push_back(new Keyboard());
}

void Input::JoystickChange(int id, int event) {
	if (event == GLFW_CONNECTED) {
		std::cout << "Joystick: " << id << " connected\n";
		if (glfwJoystickIsGamepad(id)) {
			AddGamepad(id);
		}
		else {
			// Unrecognised controller
		}
	}
	else if (event == GLFW_DISCONNECTED) {
		std::cout << "Joystick: " << id << " disconnected\n";
		for (auto i = inputDevices.begin(); i != inputDevices.end(); i++)
		{
			if ((*i)->getType() != Type::Controller) {
				continue;
			}
			Controller* c = (Controller*)(*i);
			if (c->id == id) {
				inputDevices.erase(i);
				break;
			}
		}
	}
}

Input::Controller::Controller(int _id) :
	id(_id)
{
	glfwGetGamepadState(id, &currentState);
	previousState = currentState;
}

glm::vec2 Input::Controller::getMove() const
{
	return { currentState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], currentState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] };
}

glm::vec2 Input::Controller::getLook() const
{
	return { currentState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], currentState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] };
}

float Input::Controller::getLeftTrigger() const
{
	return Utilities::mapValueTo(currentState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER], -1.0f, 1.0f, 0.0f, 1.0f);
}

float Input::Controller::getRightTrigger() const
{
	return Utilities::mapValueTo(currentState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER], -1.0f, 1.0f, 0.0f, 1.0f);
}

Input::Type Input::Controller::getType() const
{
	return Type::Controller;
}

glm::vec2 Input::Keyboard::getMove() const
{
	int horizontal = glfwGetKey(SceneManager::window, keyMoveRight) - glfwGetKey(SceneManager::window, keyMoveLeft);
	int vertical = glfwGetKey(SceneManager::window, keyMoveUp) - glfwGetKey(SceneManager::window, keyMoveDown);
	return glm::vec2(horizontal, vertical);
}

glm::vec2 Input::Keyboard::getLook() const
{
	int horizontal = glfwGetKey(SceneManager::window, keyLookRight) - glfwGetKey(SceneManager::window, keyLookLeft);
	int vertical = glfwGetKey(SceneManager::window, keyLookUp) - glfwGetKey(SceneManager::window, keyLookDown);
	return glm::vec2(horizontal, vertical);
}

float Input::Keyboard::getLeftTrigger() const
{
	float left = (float)glfwGetKey(SceneManager::window, keyLeftTrigger);
	return left;
}

float Input::Keyboard::getRightTrigger() const
{
	float right = (float)glfwGetKey(SceneManager::window, keyRightTrigger);
	return right;
}

Input::Type Input::Keyboard::getType() const
{
	return Type::Keyboard;
}
