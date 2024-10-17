#include "PrefabManager.h"

#include "Paths.h"

#include "EditorGUI.h"

#include <fstream>
#include <iostream>
#include <filesystem>

std::unordered_map<unsigned long long, toml::table> PrefabManager::loadedPrefabOriginals = {};
unsigned long long PrefabManager::selectedPrefab = 0;

void PrefabManager::Initialise()
{
	for (auto& i : std::filesystem::directory_iterator(Paths::prefabsSaveLocation))
	{
		std::ifstream file(i.path());
		toml::table temp = toml::parse(file);

		loadedPrefabOriginals[Serialisation::LoadAsUnsignedLongLong((*temp["sceneObject"].as_table())["guid"])] = temp;
	}
}

void PrefabManager::AddSceneObjectAsPrefabOrigins()
{
}

void PrefabManager::GUI()
{
	ImGui::InputScalar("Prefab Selected GUID", ImGuiDataType_U64, &selectedPrefab);
	
	
	
}
