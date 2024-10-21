#include "Bollard.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"

Bollard::Bollard(toml::table table)
{
	startsUp = Serialisation::LoadAsBool(table["startsUp"]);
	timeToLower = Serialisation::LoadAsFloat(table["timeToLower"]);
	timeToRaise = Serialisation::LoadAsFloat(table["timeToRaise"]);
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
}

void Bollard::TriggerCall(std::string tag, bool toggle)
{
	if (tag == triggerTag)
	{
		state = toggle;
	}
}

void Bollard::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Bollard##" + tag).c_str()))
	{
		ImGui::Indent();
		ImGui::Checkbox("Starts Up", &startsUp);
		ImGui::InputText("Trigger Tag", &triggerTag);
		ImGui::DragFloat("Time To Lower", &timeToLower, 0.02f, 0.0f);
		ImGui::DragFloat("Time To Raise", &timeToRaise, 0.02f, 0.0f);
		ImGui::Unindent();
	}
}

toml::table Bollard::Serialise(unsigned long long guid)
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{"startsUp", startsUp},
		{"timeToRaise", timeToRaise},
		{"timeToLower", timeToLower},
		{"triggerTag", triggerTag},
	};
}
