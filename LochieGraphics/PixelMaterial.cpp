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
	ExtraEditorGUI::ColourEdit3("Coloadsaur##" + tag, defaultColour);
	ImGui::CheckboxFlags(("Never Update##" + tag).c_str(), &flags, MaterialFlags::neverUpdate);
	ImGui::CheckboxFlags(("Gravity##" + tag).c_str(), &flags, MaterialFlags::gravity);
	ImGui::InputFloat(("Density##" + tag).c_str(), &density);
}
