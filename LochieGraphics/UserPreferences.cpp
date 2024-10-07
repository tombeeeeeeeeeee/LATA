#include "UserPreferences.h"

#include "Paths.h"

#include "EditorGUI.h"
#include "Serialisation.h"
#include "Utilities.h"

#include <fstream>

bool UserPreferences::escapeCloses = false;
std::string UserPreferences::filename = "";

void UserPreferences::GUI()
{
	if (!ImGui::Begin("User Preferences Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}
	// TODO: Selector for user prefs
	// TODO: Option to create new one

	ImGui::InputText("User Preferences Active", &filename);
	if (ImGui::Button("Save##User Prefrences")) {
		Save();
		RefreshPreferenceFile();
	}
	if (ImGui::Button("Load##User Prefrences")) {
		Load();
		RefreshPreferenceFile();
	}

	ImGui::Checkbox("Pressing Escape Quits", &escapeCloses);

	ImGui::End();
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
	};

	file << table << '\n';

	file.close();
}

void UserPreferences::Load()
{
	std::ifstream file(Paths::userPrefsSaveLocation + filename + Paths::userPrefsExtension);

	toml::table data = toml::parse(file);

	escapeCloses = Serialisation::LoadAsBool(data["escapeCloses"]);

	file.close();
}
