#include "Triggerable.h"
#include "EditorGUI.h"
#include "ExtraEditorGUI.h"
#include "Utilities.h"
#include "Serialisation.h"
#include "Collider.h"

void Triggerable::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Door##" + tag).c_str()))
	{
		ImGui::Indent();
		ImGui::InputText("Trigger ID", &triggerTag);
		ImGui::Checkbox("Both Players Needed", &bothPlayersNeeded);
		ImGui::Checkbox("False is True", &falseIsTrue);
		ImGui::Checkbox("Don't Send False", &doesntSendFalseEveryFrame);
		ImGui::Unindent();
	}
}

Triggerable::Triggerable(toml::table table)
{
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
	bothPlayersNeeded = Serialisation::LoadAsBool(table["bothPlayersNeeded"]);
	falseIsTrue = Serialisation::LoadAsBool(table["falseIsTrue"]);
	doesntSendFalseEveryFrame = Serialisation::LoadAsBool(table["doesntSendFalseEveryFrame"]);
}

toml::table Triggerable::Serialise(unsigned long long guid)
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{"triggertag", triggerTag},
		{"bothPlayersNeeded", bothPlayersNeeded},
		{"falseIsTrue", falseIsTrue},
		{"doesntSendFalseEveryFrame", doesntSendFalseEveryFrame},
	};
}

void Triggerable::OnTrigger(unsigned int mask)
{
	if (mask & (int)CollisionLayers::ecco) eccoThisFrame = true;
	if (mask & (int)CollisionLayers::sync) syncThisFrame = true;
}

