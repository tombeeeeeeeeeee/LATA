#pragma once

#include <string>

class UserPreferences
{
public:

	static std::string filename;

	static bool escapeCloses;

	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static void Load();

};

