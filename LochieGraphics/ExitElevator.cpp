#include "ExitElevator.h"

#include "LevelEditor.h"
#include "SceneManager.h"
#include "Collision.h"
#include "SceneObject.h"
#include "Paths.h"
#include "Collider.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <filesystem>

void ExitElevator::Initialise(SceneObject* so)
{
	if (so)
	{
		if (so->parts & Parts::rigidBody)
		{
			so->rigidbody()->onTrigger.push_back([this](Collision collision) { OnTrigger(collision); });
		}
	}
}

ExitElevator::ExitElevator(toml::table table)
{
	levelToLoad = Serialisation::LoadAsString(table["levelToLoad"]);
}

bool ExitElevator::Update()
{
	if (syncInExit && eccoInExit)
	{
		syncInExit = eccoInExit = false;
		((LevelEditor*)SceneManager::scene)->LoadLevel(true, levelToLoad);
		return true;
	}
	else
	{
		syncInExit = eccoInExit = false;
		return false;
	}
}

void ExitElevator::OnTrigger(Collision collision)
{
	if (collision.collisionMask & (int)CollisionLayers::ecco)
	{
		eccoInExit = true;
	}
	else if (collision.collisionMask & (int)CollisionLayers::sync)
	{
		syncInExit = true;
	}
}

void ExitElevator::GUI(SceneObject* so)
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Exit Elevator##" + tag).c_str()))
	{
		if (ImGui::Button(("Finish Level##" + tag).c_str())) eccoInExit = syncInExit = true;

		ImGui::Checkbox(("Ecco In Exit##" + tag).c_str(), &eccoInExit);
		ImGui::Checkbox(("Sync In Exit##" + tag).c_str(), &syncInExit);

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
}

toml::table ExitElevator::Serialise(unsigned long long GUID) const
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{"levelToLoad", levelToLoad}
	};
}
