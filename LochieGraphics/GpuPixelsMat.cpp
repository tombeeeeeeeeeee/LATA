#include "GpuPixelsMat.h"

#include "Utilities.h"

#include "ExtraEditorGUI.h"

#include <iostream>
#include <sstream>
#include <fstream>


std::string PixelsGPU::Material::GetMovementCode(const std::vector<Material>& mats)
{
	std::string defaults = "";
	std::string customs = "";
	for (const auto& mat : mats)
	{
		std::string caseStr = "case " + std::to_string(mat.id) + ":\n";
		if (mat.moveFunction == "default")
		{
			defaults += caseStr;
		}
		else if (!mat.moveFunction.empty())
		{
			customs += caseStr;
			customs += mat.moveFunction;
			customs += "break;\n";
		}
	}
	// Lochie: Load this from a glsl shader file
	defaults += "return lcoords + ivec2(round(pixel.vel + pixel.subPos));\nbreak;\n";
	return defaults + customs;
}

int PixelsGPU::Material::GetLargestID(const std::vector<Material>& mats)
{
	int largestID = 0;
	for (const auto& mat : mats)
	{
		largestID = glm::max(largestID, mat.id);
	}
	return largestID;
}

std::string PixelsGPU::Material::GetMaterialInfoCode(const std::vector<Material>& mats)
{
	std::string code = "";
	int largestID = GetLargestID(mats);
	const int materialCount = largestID + 1;
	code += "const int materialCount = " + std::to_string(materialCount) + ";\n";
	code += "const bool matsEmptyStatus[materialCount] = {\n";
	for (size_t i = 0; i <= largestID; i++)
	{
		const Material* mat = nullptr;
		for (const auto& search : mats)
		{
			if (search.id == i)
			{
				mat = &search;
				break;
			}
		}
		code += '\t';
		if (mat)
		{
			code += mat->empty ? "true" : "false";
		}
		else
		{
			code += "false";
		}
		code += ",\n";
	}
	code += "};\n";
	return code;
}

std::string PixelsGPU::Material::getFullPath(std::string filename)
{
	return defaultSavePath + filename + defaultExtension;
}

std::string PixelsGPU::Material::getFullPath()
{
	return getFullPath(filename);
}

PixelsGPU::Material::Material(std::string _filename)
{
	Load(_filename);
}

void PixelsGPU::Material::Load(std::string _filename)
{
	filename = _filename;
	std::string filepath = getFullPath();
	std::ifstream file(filepath);
	if (!file) {
		std::cout << "Failed to load pixels material, attempted at path: " << filepath << '\n';
		return;
	}
	toml::table data = toml::parse(file);
	Load(data);
}

void PixelsGPU::Material::SaveAsFile()
{
	std::ofstream file(getFullPath());
	file << Serialise();
	file.close();
}

toml::table PixelsGPU::Material::Serialise()
{
	return toml::table{
		{ "name", name },
		{ "preUpdateFunction", preUpdateFunction },
		{ "moveFunction", moveFunction },
		{ "defaultColour", defaultColour.Serialise() },
		{ "id", id },
		{ "density", density },
		{ "affectedByGravity", affectedByGravity },
		{ "empty", empty },
	};
}

void PixelsGPU::Material::Load(toml::table& table)
{
	name = Serialisation::LoadAsString(table["name"]);
	preUpdateFunction = Serialisation::LoadAsString(table["preUpdateFunction"]);
	moveFunction = Serialisation::LoadAsString(table["moveFunction"]);
	defaultColour.Load(*table["defaultColour"].as_table());
	id = Serialisation::LoadAsInt(table["id"]);
	density = Serialisation::LoadAsFloat(table["density"]);
	affectedByGravity = Serialisation::LoadAsBool(table["affectedByGravity"]);
	empty = Serialisation::LoadAsBool(table["empty"]);
}

void PixelsGPU::Material::Reload()
{
	Load(filename);
}

void PixelsGPU::Material::MaterialsGUI(std::vector<Material>& mats)
{
	for (auto& mat : mats)
	{
		std::string tag = Utilities::PointerToString(&mat);
		if (ImGui::CollapsingHeader((mat.name + "##" + tag).c_str()))
		{
			ExtraEditorGUI::ScopedIndent indent;
			mat.GUI();
		}
	}
}

void PixelsGPU::Material::GUI()
{
	bool shouldSave = false;
	std::string tag = Utilities::PointerToString(this);
	shouldSave |= ImGui::InputText(("Filename##" + tag).c_str(), &filename);
	shouldSave |= ImGui::InputText(("Name##" + tag).c_str(), &name);
	shouldSave |= ImGui::InputText(("Pre update function##" + tag).c_str(), &preUpdateFunction);
	shouldSave |= ImGui::InputText(("Move function##" + tag).c_str(), &moveFunction);
	shouldSave |= defaultColour.ColourPicker("Default colour");
	shouldSave |= ImGui::InputInt(("ID##" + tag).c_str(), &id);
	shouldSave |= ImGui::InputFloat(("Density##" + tag).c_str(), &density);
	shouldSave |= ImGui::Checkbox(("Does gravity##" + tag).c_str(), &affectedByGravity);
	shouldSave |= ImGui::Checkbox(("Empty##" + tag).c_str(), &empty);

	if (shouldSave)
	{
		SaveAsFile();
	}
}
