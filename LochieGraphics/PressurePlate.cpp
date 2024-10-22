#include "PressurePlate.h"
#include "Serialisation.h"
#include "imgui.h"
#include "Utilities.h"
#include "Collider.h"
#include "ExtraEditorGUI.h"
#include "TriggerSystem.h"

void PressurePlate::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Pressure Plate##" + tag).c_str()))
	{
		ImGui::Indent();

		if (ImGui::Button("Trigger"))
		{
			TriggerSystem::TriggerTag(triggerTag, true);
			eccoToggled = true;
		}

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
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{"triggerTag", triggerTag},
		{"smallType", smallType},
	};
}

void PressurePlate::OnTrigger(int layerMask)
{
	if (eccoToggled) return;
	if (smallType)
	{
		if (layerMask & ((int)CollisionLayers::ecco | (int)CollisionLayers::sync))
		{
			TriggerSystem::TriggerTag(triggerTag, true);
			triggeredThisFrame = true;
			if (layerMask & (int)CollisionLayers::ecco)
			{
				eccoToggled = true;
			}
		}
	}
	else
	{
		if (layerMask & (int)CollisionLayers::ecco)
		{
			TriggerSystem::TriggerTag(triggerTag, true);
			triggeredThisFrame = true;
		}
	}
}
