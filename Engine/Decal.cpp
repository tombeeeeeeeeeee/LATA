#include "Decal.h"
#include "ExtraEditorGUI.h"
#include "imgui.h"
#include "Serialisation.h"
#include "Material.h"
#include "ResourceManager.h"

Decal::Decal(toml::table table)
{
	depthOfDecal = Serialisation::LoadAsFloat(table["depthOfDecal"], 0.1f);
	scale = Serialisation::LoadAsFloat(table["scale"], 1.0f);
	angleTolerance = Serialisation::LoadAsFloat(table["angleTolerance"], 1.0f);
	unsigned long long matGUID = Serialisation::LoadAsUnsignedLongLong(table["matGUID"]);
	mat = ResourceManager::GetMaterial(matGUID);
}

void Decal::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Decal##" + tag).c_str()))
	{
		ImGui::Indent();
		ResourceManager::MaterialSelector("Material ", &mat, ResourceManager::defaultShader, true);
		if (!mat)ImGui::BeginDisabled();
		ImGui::DragFloat(("Decal Depth##" + tag).c_str(), &depthOfDecal);
		ImGui::SliderFloat(("Angle Tolerance##" + tag).c_str(), &angleTolerance, 0.0f, 1.0f);
		ImGui::DragFloat(("Scale##" + tag).c_str(), &scale);
		if (!mat)ImGui::EndDisabled();
		ImGui::Unindent();
	}
}

toml::table Decal::Serialise(unsigned long long guid)
{
	return toml::table{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{"matGUID", Serialisation::SaveAsUnsignedLongLong(mat->GUID)},
		{"depthOfDecal", depthOfDecal},
		{"angleTolerance", angleTolerance},
		{"scale", scale},
	};
}
