#pragma once

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

	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static void Load();

};

