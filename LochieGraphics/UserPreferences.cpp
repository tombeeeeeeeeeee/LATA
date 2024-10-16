#include "UserPreferences.h"

#include "Paths.h"

#include "EditorGUI.h"
#include "Serialisation.h"
#include "Utilities.h"

#include <fstream>

bool UserPreferences::escapeCloses = false;
std::string UserPreferences::filename = "";
UserPreferences::ModelSelectMode UserPreferences::modelSelectMode = UserPreferences::ModelSelectMode::assets;
std::string UserPreferences::defaultLevelLoad = "";
bool UserPreferences::rememberLastLevel = true;
bool UserPreferences::loadDefaultLevel = true;

void UserPreferences::GUI()
{
	// TODO: Selector for user prefs
	// TODO: Option to create new one

	ImGui::InputText("User Preferences Active", &filename);
	if (ImGui::Button("Save##User Prefrences")) {
		Save();
		RefreshPreferenceFile();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##User Prefrences")) {
		Load();
		RefreshPreferenceFile();
	}

	ImGui::Checkbox("Pressing Escape Quits", &escapeCloses);

	ImGui::Combo("Model Chooser Mode", (int*)&modelSelectMode, "Loaded\0Assets\0\0");

	ImGui::Checkbox("Load Default Level", &loadDefaultLevel);
	ImGui::InputText("Default Level Load", &defaultLevelLoad);
	ImGui::Checkbox("Rememeber Last Level", &rememberLastLevel);
}

void UserPreferences::Initialise()
{
	std::ifstream lastUsed(Paths::lastUsedUserPrefsFilePath);

	if (!lastUsed) {
		return;
	}

	std::string newFilename = Utilities::FileToString(Paths::lastUsedUserPrefsFilePath);

	if (newFilename != "") {
		filename = newFilename;
		Load();
	}

	lastUsed.close();
}

void UserPreferences::RefreshPreferenceFile()
{
	std::ofstream file(Paths::lastUsedUserPrefsFilePath);

	file << filename;

	file.close();
}

void UserPreferences::Save()
{
	if (filename == "") {
		// TODO: Warning here
		return;
	}

	std::ofstream file(Paths::userPrefsSaveLocation + filename + Paths::userPrefsExtension);

	toml::table table{
		{ "escapeCloses", escapeCloses },
		{ "modelSelectMode", (int)modelSelectMode },
		{ "loadDefaultLevel", loadDefaultLevel },
		{ "defaultLevelLoad", defaultLevelLoad },
		{ "rememberLastLevel", rememberLastLevel},
	};

	file << table << '\n';

	file.close();
}

void UserPreferences::Load()
{
	std::ifstream file(Paths::userPrefsSaveLocation + filename + Paths::userPrefsExtension);

	toml::table data = toml::parse(file);

	escapeCloses = Serialisation::LoadAsBool(data["escapeCloses"]);
	modelSelectMode = (ModelSelectMode)Serialisation::LoadAsInt(data["modelSelectMode"]);
	loadDefaultLevel = Serialisation::LoadAsBool(data["loadDefaultLevel"]);
	defaultLevelLoad = Serialisation::LoadAsString(data["defaultLevelLoad"]);
	rememberLastLevel = Serialisation::LoadAsBool(data["rememberLastLevel"]);

	file.close();
}
