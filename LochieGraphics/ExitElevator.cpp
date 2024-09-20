#include "ExitElevator.h"

#include "LevelEditor.h"
#include "SceneManager.h"
#include "Collision.h"
#include "SceneObject.h"
#include "Paths.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <filesystem>

void ExitElevator::Initialise(SceneObject* so)
{
	if (so->parts & Parts::rigidBody && !hasBeenBound)
	{
		hasBeenBound = true;
		so->rigidbody()->onTrigger.push_back([this](Collision collision) { OnTrigger(collision); });
	}
}

ExitElevator::ExitElevator(toml::table table)
{
	levelToLoad = Serialisation::LoadAsString(table["levelToLoad"]);
}

void ExitElevator::Update()
{
	if (syncInExit && eccoInExit)
	{
		((LevelEditor*)SceneManager::scene)->LoadLevel(levelToLoad);
	}
	else
	{
		syncInExit = eccoInExit = false;
	}
}

void ExitElevator::OnTrigger(Collision collision)
{
	if (collision.collisionMask & Parts::ecco)
	{
		eccoInExit = true;
	}
	else if (collision.collisionMask & Parts::sync)
	{
		syncInExit = true;
	}
}

void ExitElevator::GUI(SceneObject* so)
{
	if (hasBeenBound)ImGui::BeginDisabled();
	if (ImGui::Button("Bind OnTrigger"))
	{
		Initialise(so);
	}
	if (hasBeenBound)ImGui::EndDisabled();
	ImGui::Checkbox("Ecco In Exit", &eccoInExit);
	ImGui::Checkbox("Sync In Exit", &syncInExit);

	std::vector<std::string> loadPaths = {};
	std::vector<std::string*> loadPathsPointers = {};
	loadPaths.clear();
	loadPathsPointers.clear();

	for (auto& i : std::filesystem::directory_iterator(Paths::levelsPath))
	{
		loadPaths.push_back(i.path().generic_string().substr(Paths::levelsPath.size()));
		if (loadPaths.back().substr(loadPaths.back().size() - Paths::levelExtension.size()) != Paths::levelExtension) {
			loadPaths.erase(--loadPaths.end());
			continue;
		}
		loadPaths.back() = loadPaths.back().substr(0, loadPaths.back().size() - Paths::levelExtension.size());
	}
	for (auto& i : loadPaths)
	{
		loadPathsPointers.push_back(&i);
	}

	std::string* selected = &levelToLoad;
	if (ExtraEditorGUI::InputSearchBox(loadPathsPointers.begin(), loadPathsPointers.end(), &selected, "Filename", Utilities::PointerToString(&loadPathsPointers), false)) {
		ImGui::CloseCurrentPopup();
		levelToLoad = *selected;
	}
}

toml::table ExitElevator::Serialise(unsigned long long GUID) const
{
	return toml::table
	{
		{"levelToLoad", levelToLoad}
	};
}
