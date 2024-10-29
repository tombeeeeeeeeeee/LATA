#pragma once

// TODO: This is only here for the window mode, would be nice to remove
#include "SceneManager.h"

#include <string>

class UserPreferences
{
public:
	static float camMove;
	static float camRotate;
	static float camOrbit;
	static float camBoomTruck;
	static float camMoveDolly;
	static float camScrollDolly;
	static float orthScrollSpeed;

	static std::string filename;

	static bool escapeCloses;

	static bool clearSearchBar;
	
	enum class ModelSelectMode {
		loaded,
		assets
	};
	static ModelSelectMode modelSelectMode;

	static bool immortal;
	static bool loadDefaultLevel;
	static std::string defaultLevelLoad;
	static bool rememberLastLevel;
	static bool enterPlayModeOnStart;
	static WindowModes windowedStartMode;

	static std::string defaultCameraSystemLoad;
	static std::string defaultEnemySystemLoad;
	static std::string defaultHealthSystemLoad;

	static float defaultGlobalVolume;

	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static void Load();
};

