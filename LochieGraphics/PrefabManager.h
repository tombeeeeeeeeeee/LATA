#pragma once

#include <unordered_map>

// TODO: Try to remove this header
#include "Serialisation.h"

class SceneObject;

class PrefabManager
{
public:
	static std::unordered_map<unsigned long long, toml::table> loadedPrefabOriginals;

	static unsigned long long selectedPrefab;

	static void Initialise();

	static void AddSceneObjectAsPrefabOrigins();

	static void RefreshPrefabInstancesOf(unsigned long long GUID);
	static void RefreshAllPrefabInstances();

	static void GUI();

};

