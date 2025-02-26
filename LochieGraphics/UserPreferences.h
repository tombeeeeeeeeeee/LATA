#pragma once

#include <string>

#include "Maths.h"

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
	
	static bool saveOnLevelPlay;
	static bool loadDefaultLevel;
	static std::string defaultLevelLoad;
	static bool rememberLastLevel;
	static bool enterPlayModeOnStart;
	static int windowedStartMode;

	static std::string defaultStyleLoad;

	static bool advancedTransformInfo;
	static bool showSelectedBox;

	static float defaultGlobalVolume;

	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static bool Load();
};

