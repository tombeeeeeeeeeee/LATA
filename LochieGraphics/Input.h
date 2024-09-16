#pragma once

#include "Graphics.h"
#include "Maths.h"

#include <vector>

class Input
{
public:
	static Input* input;

	enum class Type {
		Keyboard,
		Controller
	};

	class InputDevice {
	public:
		virtual glm::vec2 getMove() const = 0;
		virtual glm::vec2 getLook() const = 0;
		virtual float getLeftTrigger()  const = 0;
		virtual float getRightTrigger() const = 0;
		virtual bool getButton1() const = 0;
		virtual bool getButton2() const = 0;
		virtual bool getButton3() const = 0;
		virtual bool getButton4() const = 0;
		virtual Type getType() const = 0;
	};

	class Keyboard : public InputDevice {
	public:
		const int keyMoveUp = GLFW_KEY_W;
		const int keyMoveLeft = GLFW_KEY_A;
		const int keyMoveDown = GLFW_KEY_S;
		const int keyMoveRight = GLFW_KEY_D;

		const int keyLookUp = GLFW_KEY_UP;
		const int keyLookLeft = GLFW_KEY_LEFT;
		const int keyLookDown = GLFW_KEY_DOWN;
		const int keyLookRight = GLFW_KEY_DOWN;

		const int keyLeftTrigger = GLFW_KEY_LEFT_CONTROL;
		const int keyRightTrigger = GLFW_KEY_LEFT_SHIFT;

		const int keyButton1 = GLFW_KEY_E;
		const int keyButton2 = GLFW_KEY_Q;
		const int keyButton3 = GLFW_KEY_F;
		const int keyButton4 = GLFW_KEY_R;

		glm::vec2 getMove() const override;
		glm::vec2 getLook() const override;
		float getLeftTrigger() const override;
		float getRightTrigger() const override;
		bool getButton1() const override;
		bool getButton2() const override;
		bool getButton3() const override;
		bool getButton4() const override;
		Type getType() const override;
	};

	class Controller : public InputDevice {
	public:
		int id;
		GLFWgamepadstate previousState;
		GLFWgamepadstate currentState;

		Controller(int _id);

		glm::vec2 getMove() const override;
		glm::vec2 getLook() const override;
		float getLeftTrigger() const override;
		float getRightTrigger() const override;
		bool getButton1() const override;
		bool getButton2() const override;
		bool getButton3() const override;
		bool getButton4() const override;
		Type getType() const override;
	};

	std::vector<InputDevice*> inputDevices;


	void Initialise();
	void AddGamepad(int id);

	void AddKeyboard();

	static void JoystickCallback(int id, int event);
	void JoystickChange(int id, int event);

	void ShowAllControllerSlotStatuses();

	void Update();

	bool windowOpen = true;
	void GUI();
};

