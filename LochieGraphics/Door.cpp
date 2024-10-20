#include "Door.h"
#include "Serialisation.h"
#include "ExtraEditorGUI.h"

Door::Door(toml::table table)
{
	left = Serialisation::LoadAsBool(table["left"]);
	closed = Serialisation::LoadAsBool(table["closed"]);
	timeToClose = Serialisation::LoadAsFloat(table["timeToClose"]);
	timeToOpen = Serialisation::LoadAsFloat(table["timeToOpen"]);
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
}

toml::table Door::Serialise(unsigned long long guid)
{
	return toml::table{
		{"guid", Serialisation::SaveAsUnsignedInt(guid)},
		{"left", left},
		{"closed", closed},
		{"timeToClose", timeToClose},
		{"timeToOpen", timeToOpen},
		{"triggerTag", triggerTag},
	};
}

void Door::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Door##" + tag).c_str()))
	{
		ImGui::Indent();
		ImGui::Checkbox("Starts Closed", &closed);
		ImGui::Checkbox("Moves Left", &left);
		ImGui::SameLine();
		bool right = !left;
		ImGui::Checkbox("Moves Right", &right);
		ImGui::InputText("Trigger Tag", &triggerTag);
		ImGui::DragFloat("Time To Close", &timeToClose, 0.02f, 0.0f);
		ImGui::DragFloat("Time To Open", &timeToClose, 0.02f, 0.0f);
	}
}
