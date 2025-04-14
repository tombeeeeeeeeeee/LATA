#include "PixelMaterial.h"

#include "Utilities.h"
#include "ExtraEditorGUI.h"

Pixels::Material::Material(std::string _name, glm::u8vec3(_colour), float _density, unsigned int _flags) :
	name(_name),
	defaultColour(_colour),
	density(_density),
	flags(_flags)
{
}

void Pixels::Material::GUI()
{
	std::string tag = Utilities::PointerToString(this);

	ImGui::InputText(("Name##" + tag).c_str(), &name);
	ExtraEditorGUI::ColourEdit3("Default Colour##" + tag, defaultColour);
	ImGui::CheckboxFlags(("Never Update##" + tag).c_str(), &flags, MaterialFlags::neverUpdate);
	ImGui::CheckboxFlags(("Gravity##" + tag).c_str(), &flags, MaterialFlags::gravity);
	ImGui::InputFloat(("Density##" + tag).c_str(), &density);
	ImGui::SliderFloat(("Half Angle Spread##" + tag).c_str(), &halfAngleSpread, 0.0f, PI / 2);
}
