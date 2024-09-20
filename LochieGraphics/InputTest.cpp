#include "InputTest.h"

#include "Utilities.h"

InputTest::InputTest()
{
}

void InputTest::Start()
{
	lights.insert(lights.end(), {
	&directionalLight,
	&spotlight,
	&pointLights[0],
	&pointLights[1],
	&pointLights[2],
	&pointLights[3],
	});

	input.Initialise();
	input.ShowAllControllerSlotStatuses();

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
