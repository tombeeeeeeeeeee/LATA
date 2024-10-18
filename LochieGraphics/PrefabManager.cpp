#include "PrefabManager.h"

#include "Paths.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneObject.h"

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

void PrefabManager::RefreshPrefabInstancesOf(unsigned long long GUID)
{
	toml::table& table = loadedPrefabOriginals.at(GUID);
	for (auto& i : SceneManager::scene->sceneObjects)
	{
		if (i.second->prefabBase != GUID) {
			continue;
		}
		i.second->LoadFromPrefab(table);
	}
}

void PrefabManager::RefreshAllPrefabInstances()
{
	for (auto& i : SceneManager::scene->sceneObjects)
	{
		if (i.second->prefabStatus != SceneObject::PrefabStatus::prefabInstance) {
			continue;
		}
		i.second->LoadFromPrefab(loadedPrefabOriginals.at(i.second->prefabBase));
	}
}

void PrefabManager::PrefabSelector()
{
	// TODO: Make better, search bar, name instead
	ImGui::InputScalar("Selected Prefab", ImGuiDataType_U64, &selectedPrefab);
}

void PrefabManager::GUI()
{
	PrefabSelector();

	if (ImGui::Button("Refresh Instances of Selected")) {
		RefreshPrefabInstancesOf(selectedPrefab);
	}
	if (ImGui::Button("Refresh All Prefab Instances")) {
		RefreshAllPrefabInstances();
	}
}
