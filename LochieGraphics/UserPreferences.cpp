#include "UserPreferences.h"

#include "Scene.h"
#include "Paths.h"
#include "SceneManager.h"

#include "ImGuiStyles.h"
#include "EditorGUI.h"
#include "Serialisation.h"
#include "Utilities.h"

#include <fstream>
#include <iostream>

bool UserPreferences::escapeCloses = false;
std::string UserPreferences::filename = "";
std::string UserPreferences::defaultLevelLoad = "";
bool UserPreferences::rememberLastLevel = true;
bool UserPreferences::loadDefaultLevel = true;
bool UserPreferences::enterPlayModeOnStart = false;
int UserPreferences::windowedStartMode = (int)WindowModes::maximised;
float UserPreferences::defaultGlobalVolume = 1.0f;
float UserPreferences::camMove;
float UserPreferences::camRotate;
float UserPreferences::camOrbit;
float UserPreferences::camBoomTruck;
float UserPreferences::camMoveDolly;
float UserPreferences::camScrollDolly;
float UserPreferences::orthScrollSpeed;
bool UserPreferences::saveOnLevelPlay;
std::string UserPreferences::defaultStyleLoad = "OtherStyle";
bool UserPreferences::clearSearchBar = true;
bool UserPreferences::advancedTransformInfo = false;
bool UserPreferences::showSelectedBox = true;

void UserPreferences::GUI()
{
	// TODO: Selector for user prefs
	// TODO: Option to create new one

	bool shouldSave = false;

	if (ImGui::InputText("User Preferences Active", &filename, ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (!Load()) {
			shouldSave = true; 
		}
	}
	ImGui::BeginDisabled();
	ImGui::Button("Save##User Prefrences");
	ImGui::EndDisabled();
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::BeginTooltip();
		ImGui::Text("User Preferences auto save");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##User Prefrences")) {
		Load();
	}

	if (ImGui::Checkbox("Pressing Escape Quits", &escapeCloses)) { shouldSave = true; }

	if (ImGui::Combo("Default Windowed Mode", &windowedStartMode, "Windowed\0Borderless Fullscreen\0Maximised\0\0")) { shouldSave = true; }

	if (ImGui::SliderFloat("Default Global Audio Volume", &defaultGlobalVolume, 0.0f, 2.0f)) {
		shouldSave = true;
		SceneManager::scene->audio.soloud.setGlobalVolume(defaultGlobalVolume);
	}

	if (ImGui::Checkbox("Clear search bar on search bar creation", &clearSearchBar)) { shouldSave = true; }

	if (ImGui::Checkbox("Show Advanced Transform Information", &advancedTransformInfo)) { shouldSave = true; }


	if (ImGuiStyles::Selector()) {
		defaultStyleLoad = ImGuiStyles::filename;
		shouldSave = true;
	}
	ImGui::Indent();
	if (ImGui::Button("Open Style Editor")) {
		SceneManager::scene->gui.showStyleMenu = true;
		// TODO: Set window focus incase window was already open
	}
	ImGui::Unindent();

	if (ImGui::Checkbox("Show Selected Model Min-Maxes", &showSelectedBox)) { shouldSave = true; }

	if (ImGui::CollapsingHeader("Camera Move Speeds")) {
		if (ImGui::DragFloat("Orthographic Zoom Speed##Camera", &orthScrollSpeed, 0.1f, 0.0f, FLT_MAX)) { shouldSave = true; }
		ImGui::Text("Editor Values");
		ImGui::Indent();
		if (ImGui::DragFloat("Movement##Camera", &camMove, 0.1f, 0.0f, FLT_MAX)) { shouldSave = true; }
		if (ImGui::DragFloat("Rotate##Camera", &camRotate, 0.01f, 0.0f, FLT_MAX)) { shouldSave = true; }
		ImGui::Unindent();
		ImGui::Text("Art Values");
		ImGui::Indent();
		float orbit = camOrbit * 10000.0f;
		if (ImGui::DragFloat("Orbit##Camera", &orbit, 0.05f, 0.0f, FLT_MAX, "%.f")) {
			camOrbit = orbit / 10000.0f;
			shouldSave = true;
		}
		if (ImGui::DragFloat("Move##Camera", &camBoomTruck, 0.01f, 0.0f, FLT_MAX)) { shouldSave = true; }
		if (ImGui::DragFloat("Mouse Dolly##Camera", &camMoveDolly, 0.01f, 0.0f, FLT_MAX)) { shouldSave = true; }
		if (ImGui::DragFloat("Scroll Dolly##Camera", &camScrollDolly, 0.1f, 0.0f, FLT_MAX)) { shouldSave = true; }
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Level Editor")) {
		ImGui::Indent();
		if (ImGui::Checkbox("Load Default Level", &loadDefaultLevel)) { shouldSave = true; }
		if (!loadDefaultLevel) { 
			ImGui::BeginDisabled();
			ImGui::Indent();
		}
		if (ImGui::InputText("Default Level Load", &defaultLevelLoad)) { shouldSave = true; }
		if (ImGui::Checkbox("Rememeber Last Level", &rememberLastLevel)) { shouldSave = true; }
		if (ImGui::Checkbox("Enter Play Mode On Launch", &enterPlayModeOnStart)) { shouldSave = true; }
		if (!loadDefaultLevel) { 
			ImGui::EndDisabled();
			ImGui::Unindent();
		}
		if (ImGui::Checkbox("Save on level play", &saveOnLevelPlay)) { shouldSave = true; }
		ImGui::Unindent();
	}
	if (shouldSave) {
		Save();
	}
}

void UserPreferences::Initialise()
{
	std::ifstream lastUsed(Paths::lastUsedUserPrefsFilePath);

	std::string newFilename;

	if (lastUsed) {
		newFilename = Utilities::FileToString(Paths::lastUsedUserPrefsFilePath);
	}
	else {
		newFilename = "Default";
	}

	lastUsed.close();
	
	filename = newFilename;
	Load();

	// Switching to Preferenced windowed mode is done in SceneManager Start

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
		{ "loadDefaultLevel", loadDefaultLevel },
		{ "defaultLevelLoad", defaultLevelLoad },
		{ "rememberLastLevel", rememberLastLevel },
		{ "enterPlayModeOnStart", enterPlayModeOnStart },
		{ "windowedStartMode", windowedStartMode },
		{ "defaultGlobalVolume", defaultGlobalVolume },
		{ "camMove", camMove},
		{ "camRotate", camRotate},
		{ "camOrbit", camOrbit},
		{ "camBoomTruck", camBoomTruck},
		{ "camMoveDolly", camMoveDolly},
		{ "camScrollDolly", camScrollDolly},
		{ "orthScrollSpeed", orthScrollSpeed},
		{ "clearSearchBar", clearSearchBar },
		{ "advancedTransformInfo", advancedTransformInfo},
		{ "defaultStyleLoad", defaultStyleLoad },
		{ "showSelectedBox", showSelectedBox },
		{ "directionalLightDirection", Serialisation::SaveAsVec3(SceneManager::scene->directionalLight.direction)},
		{ "directionalLightColour", Serialisation::SaveAsVec3(SceneManager::scene->directionalLight.colour)},
		{ "saveOnLevelPlay", saveOnLevelPlay },
	};

	file << table << '\n';

	file.close();

	RefreshPreferenceFile();

	std::cout << "saved user prefs\n";
}

bool UserPreferences::Load()
{
	std::ifstream file(Paths::userPrefsSaveLocation + filename + Paths::userPrefsExtension);

	if (!file) {
		std::cout << "Failed to load any user prefs!\n";
		return false;
	}

	toml::table data = toml::parse(file);

	escapeCloses = Serialisation::LoadAsBool(data["escapeCloses"]);
	loadDefaultLevel = Serialisation::LoadAsBool(data["loadDefaultLevel"]);
	defaultLevelLoad = Serialisation::LoadAsString(data["defaultLevelLoad"]);
	rememberLastLevel = Serialisation::LoadAsBool(data["rememberLastLevel"]);
	enterPlayModeOnStart = Serialisation::LoadAsBool(data["enterPlayModeOnStart"]);
	windowedStartMode = Serialisation::LoadAsInt(data["windowedStartMode"]);
	// TODO: Set the volume here to this
	defaultGlobalVolume = Serialisation::LoadAsFloat(data["defaultGlobalVolume"], 1.0f);
	camMove = Serialisation::LoadAsFloat(data["camMove"], 250.0f);
	camRotate = Serialisation::LoadAsFloat(data["camRotate"], 0.01f);
	camOrbit = Serialisation::LoadAsFloat(data["camOrbit"], 0.01f);
	camBoomTruck = Serialisation::LoadAsFloat(data["camBoomTruck"], 0.05f);
	camMoveDolly = Serialisation::LoadAsFloat(data["camMoveDolly"], 0.1f);
	camScrollDolly = Serialisation::LoadAsFloat(data["camScrollDolly"], 0.1f);
	orthScrollSpeed = Serialisation::LoadAsFloat(data["orthScrollSpeed"], 200.0f);
	clearSearchBar = Serialisation::LoadAsBool(data["clearSearchBar"]);
	advancedTransformInfo = Serialisation::LoadAsBool(data["advancedTransformInfo"], false);
	defaultStyleLoad = Serialisation::LoadAsString(data["defaultStyleLoad"], "Default");
	showSelectedBox = Serialisation::LoadAsBool(data["showSelectedBox"], true);
	saveOnLevelPlay = Serialisation::LoadAsBool(data["saveOnLevelPlay"], true);

	file.close();

	RefreshPreferenceFile();

	ImGuiStyles::filename = defaultStyleLoad;
	ImGuiStyles::Load();

	return true;
}
