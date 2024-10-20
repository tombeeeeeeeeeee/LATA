#include "PressurePlate.h"
#include "Serialisation.h"
#include "imgui.h"
#include "Utilities.h"
#include "ExtraEditorGUI.h"

void PressurePlate::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Prerssure Plate##" + tag).c_str()))
	{
		ImGui::Indent();
		const char* pressurePlateTypes[] = {"Large", "Small"};
		const char* currType = pressurePlateTypes[smallType ? 1 : 0];
		if (ImGui::BeginCombo("Pressure Plate Type", currType))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected;
				if (smallType)
					isSelected = i == 1;
				else
					isSelected = i == 0;

				if (ImGui::Selectable(pressurePlateTypes[i], isSelected))
					smallType = i == 1;

				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::InputText("Trigger ID", &triggerTag);
	}
}

PressurePlate::PressurePlate(toml::table table)
{
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
	smallType = Serialisation::LoadAsBool(table["smallType"]);
}

toml::table PressurePlate::Serialise(unsigned long long guid)
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedInt(guid)},
		{"triggerTag", triggerTag},
		{"smallType", smallType},
	};
}

void PressurePlate::OnTriggerEnter(int layerMask)
{
}
