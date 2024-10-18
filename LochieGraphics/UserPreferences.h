#pragma once

// TODO: This is only here for the window mode, would be nice to remove
#include "SceneManager.h"

#include <string>

class UserPreferences
{
public:

	static std::string filename;

	static bool escapeCloses;
	
	enum class ModelSelectMode {
		loaded,
		assets
	};
	static ModelSelectMode modelSelectMode;

	static bool loadDefaultLevel;
	static std::string defaultLevelLoad;
	static bool rememberLastLevel;
	static bool enterPlayModeOnStart;
	static WindowModes windowedStartMode;

	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static void Load();
};

