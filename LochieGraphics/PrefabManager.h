#pragma once

#include <unordered_map>

// TODO: Try to remove this header
#include "Serialisation.h"

class SceneObject;

//namespace toml {
//	inline namespace v3 {
//		class table;
//	}
//};

class PrefabManager
{
public:
	static std::unordered_map<unsigned long long, toml::table> loadedPrefabOriginals;

	static unsigned long long selectedPrefab;

	static void Initialise();

	static void AddSceneObjectAsPrefabOrigins();

	static void GUI();

	//void RefreshAllPrefabInstances();
};

