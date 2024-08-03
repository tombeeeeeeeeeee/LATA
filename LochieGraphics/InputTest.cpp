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
	renderSystem->Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);

}

void InputTest::GUI()
{
	if (ImGui::Begin("Input Debug")) {
		if (ImGui::Button("Show all controller connections")) {
			input.ShowAllControllerSlotStatuses();
		}
		ImGui::BeginDisabled();
		for (Input::Inputter* i : input.inputters)
		{
			glm::vec2 move = i->getMove();
			glm::vec2 look = i->getLook();
			ImGui::DragFloat2(("Move##" + Utilities::PointerToString(i)).c_str(), &(move.x));
			//ImGui::SameLine();
			ImGui::DragFloat2(("Look##" + Utilities::PointerToString(i)).c_str(), &(look.x));
		}
		ImGui::EndDisabled();
	}
	ImGui::End();
}
