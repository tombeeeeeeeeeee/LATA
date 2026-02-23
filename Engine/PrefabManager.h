#pragma once

#include <unordered_map>

// TODO: Try to remove this header
#include "Serialisation.h"

class SceneObject;

class PrefabManager
{
public:
	struct PrefabHolder {
		std::pair<const unsigned long long, toml::table>* loadedPrefab;
		operator std::string();
	};

	static std::unordered_map<unsigned long long, toml::table> loadedPrefabOriginals;

	static unsigned long long selectedPrefab;

	static void Initialise();

	static void RefreshPrefabInstancesOf(unsigned long long GUID);
	static void RefreshAllPrefabInstances();
	static void SaveAllPrefabOrigins();

	static void PrefabSelector();
	static void GUI();

};

