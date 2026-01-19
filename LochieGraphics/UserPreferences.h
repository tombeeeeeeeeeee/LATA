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

	static int windowedStartMode;

	static std::string defaultStyleLoad;

	static bool advancedTransformInfo;
	static bool showSelectedBox;

	static float defaultGlobalVolume;

	static float fontGlobalScale;
	static void GUI();

	static void Initialise();

	static void RefreshPreferenceFile();

	static void Save();
	static bool Load();
};

