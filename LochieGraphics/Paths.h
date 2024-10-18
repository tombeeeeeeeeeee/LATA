#pragma once

#include <string>

namespace Paths {

	const std::string importTextureLocation = "images/";
	const std::string importModelLocation = "models/";

	// TODO: Names here aren't consistent, plurality
	const std::string levelsPath = "Levels/";
	const std::string rendererSaveLocation = "Assets/Renderers/";
	const std::string materialSaveLocation = "Assets/Materials/";
	const std::string textureSaveLocation = "Assets/Textures/";
	const std::string modelSaveLocation = "Assets/Models/";
	const std::string prefabsSaveLocation = "Assets/Prefabs/";
	const std::string shadersSaveLocation = "Assets/Shaders/";

	const std::string userPrefsSaveLocation = "UserPrefs/";

	const std::string rendererExtension = ".renderer";
	const std::string materialExtension = ".material";
	const std::string textureExtension = ".texture";
	const std::string modelExtension = ".model";
	const std::string levelExtension = ".level";
	const std::string prefabExtension = ".prefab";
	const std::string imageExtension = ".png";
	const std::string userPrefsExtension = ".prefs";
	const std::string shaderExtension = ".shader";

	const std::string lastUsedUserPrefsFilePath = "CurrentUserPrefs.info";

}