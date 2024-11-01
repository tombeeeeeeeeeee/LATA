#include "PrefabManager.h"

#include "Paths.h"
#include "Scene.h"
#include "SceneObject.h"
#include "SceneManager.h"

#include "ExtraEditorGUI.h"

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

void PrefabManager::RefreshPrefabInstancesOf(unsigned long long GUID)
{
	auto search = loadedPrefabOriginals.find(GUID);
	if (search == loadedPrefabOriginals.end()) { return; }
	for (auto& i : SceneManager::scene->sceneObjects)
	{
		if (i.second->prefabBase != GUID) {
			continue;
		}
		i.second->LoadFromPrefab(search->second);
	}
}

void PrefabManager::RefreshAllPrefabInstances()
{
	for (auto& i : SceneManager::scene->sceneObjects)
	{
		if (i.second->prefabStatus != SceneObject::PrefabStatus::prefabInstance) {
			continue;
		}
		auto search = loadedPrefabOriginals.find(i.second->prefabBase);
		if (search == loadedPrefabOriginals.end()) {
			i.second->prefabStatus = SceneObject::PrefabStatus::missing;
		}
		else {
			i.second->LoadFromPrefab(search->second);
		}
	}
}

void PrefabManager::SaveAllPrefabOrigins()
{
	for (auto& i : SceneManager::scene->sceneObjects)
	{
		if (i.second->prefabStatus != SceneObject::PrefabStatus::prefabOrigin) {
			continue;
		}
		i.second->SaveAsPrefab();
	}
}

void PrefabManager::PrefabSelector()
{
	PrefabHolder* selected = nullptr;
	std::vector<PrefabHolder> prefabHolders;
	unsigned int index = 0;
	for (auto& i : loadedPrefabOriginals)
	{
		prefabHolders.push_back(PrefabHolder{ &i });
	}
	std::vector<PrefabHolder*> prefabHoldersPointers;
	for (auto i = 0; i < prefabHolders.size(); i++)
	{
		if (selectedPrefab == prefabHolders.at(i).loadedPrefab->first) { selected = &prefabHolders.at(i); }
		prefabHoldersPointers.push_back(&prefabHolders[i]);
	}

	if (ExtraEditorGUI::InputSearchBox(prefabHoldersPointers.begin(), prefabHoldersPointers.end(), &selected, "Prefab Selected", "daTag", true)) {
		if (selected) {
			selectedPrefab = selected->loadedPrefab->first;
		}
	}
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
	if (ImGui::Button("Save Any Origins")) {
		SaveAllPrefabOrigins();
	}
}

PrefabManager::PrefabHolder::operator std::string()
{
	return Serialisation::LoadAsString((*loadedPrefab->second["sceneObject"].as_table())["name"]);
}
