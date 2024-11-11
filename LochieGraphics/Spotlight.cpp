#include "Spotlight.h"


#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"


Spotlight::Spotlight(toml::table table) : PointLight::PointLight(table)
{
	direction = Serialisation::LoadAsVec3(table["direction"], {1.0f,0.0f,0.0f});
	cutOff = Serialisation::LoadAsFloat(table["cutOff"]);
	outerCutOff = Serialisation::LoadAsFloat(table["outerCutOff"]);
}

void Spotlight::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Spotlight##" + tag).c_str()))
	{
		ImGui::Indent();
		std::string tag = Utilities::PointerToString(this);
		ImGui::Checkbox(("On##" + tag).c_str(), &on);
		ImGui::ColorPicker3(("Colour##" + tag).c_str(), &colour[0]);
		//Tom Changed this, feel free to set it back.
		float _range = 100.0f * range;
		if (ImGui::DragFloat(("Range##" + tag).c_str(), &_range, 0.1f))
			SetRange(_range);
		ImGui::SliderFloat(("Cut off##" + tag).c_str(), &cutOff, 0.f, 1.f);
		ImGui::SliderFloat(("Outer cut off##" + tag).c_str(), &outerCutOff, 0.f, 1.f);
		if (ImGui::DragFloat3(("Direction" + tag).c_str(), &direction[0]))
		{
			direction = glm::normalize(direction);
		}
		ImGui::InputText(("Trigger ID##" + tag).c_str(), &triggerTag);
		ImGui::Checkbox(("Can Be Triggered##" + tag).c_str(), &canBeTriggered);
		ImGui::Unindent();
	}
}

toml::table Spotlight::Serialise(unsigned long long guid) const
{
	return toml::table{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{ "quadratic", quadratic },
		{ "linear", linear },
		{ "range", range },
		{ "colour", Serialisation::SaveAsVec3(colour) },
		{ "triggerTag", triggerTag },
		{ "on", on },
		{ "canBeTriggered", canBeTriggered },
		{ "direction",Serialisation::SaveAsVec3(direction) },
		{ "cutOff", cutOff },
		{ "outerCutOff", outerCutOff },
	};
}


