#include "InputTest.h"

#include "Utilities.h"

InputTest::InputTest()
{
}

void InputTest::Start()
{
	input.Initialise();
	input.ShowAllControllerSlotStatuses();
	directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });
	input.AddKeyboard();
}

void InputTest::Update(float delta)
{
	input.Update();
}

void InputTest::Draw()
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);

}

void InputTest::GUI()
{
}
