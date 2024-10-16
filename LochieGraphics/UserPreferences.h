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

	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static void Load();

};

