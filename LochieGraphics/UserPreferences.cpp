#include "UserPreferences.h"

#include "SceneManager.h"
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
bool UserPreferences::enterPlayModeOnStart = false;
WindowModes UserPreferences::windowedStartMode = WindowModes::maximised;
float UserPreferences::defaultGlobalVolume = 1.0f;

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

	ImGui::Combo("Default Windowed Mode", (int*)&windowedStartMode, "Windowed\0Borderless Fullscreen\0Maximised\0\0");

	ImGui::SliderFloat("Default Global Audio Volume", &defaultGlobalVolume, 0.0f, 2.0f);

	if (ImGui::CollapsingHeader("Level Editor")) {
		ImGui::Indent();
		ImGui::Checkbox("Load Default Level", &loadDefaultLevel);
		if (!loadDefaultLevel) { 
			ImGui::BeginDisabled();
			ImGui::Indent();
		}
		ImGui::InputText("Default Level Load", &defaultLevelLoad);
		ImGui::Checkbox("Rememeber Last Level", &rememberLastLevel);
		ImGui::Checkbox("Enter Play Mode On Launch", &enterPlayModeOnStart);
		if (!loadDefaultLevel) { 
			ImGui::EndDisabled();
			ImGui::Unindent();
		}
		ImGui::Unindent();
	}

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

	// Switching to Preferenced windowed mode is done in SceneManager Start

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
		{ "rememberLastLevel", rememberLastLevel },
		{ "enterPlayModeOnStart", enterPlayModeOnStart },
		{ "windowedStartMode", (int)windowedStartMode },
		{ "defaultGlobalVolume", defaultGlobalVolume },
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
	enterPlayModeOnStart = Serialisation::LoadAsBool(data["enterPlayModeOnStart"]);
	windowedStartMode = (WindowModes)Serialisation::LoadAsInt(data["windowedStartMode"]);
	defaultGlobalVolume = Serialisation::LoadAsFloat(data["defaultGlobalVolume"], 1.0f);

	file.close();
}
