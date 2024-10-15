#pragma once

#include <string>

namespace Paths {

	const std::string importTextureLocation = "images/";
	const std::string importModelLocation = "models/";

	const std::string levelsPath = "Levels/";
	const std::string rendererSaveLocation = "Assets/Renderers/";
	const std::string materialSaveLocation = "Assets/Materials/";
	const std::string textureSaveLocation = "Assets/Textures/";
	const std::string modelSaveLocation = "Assets/Models/";

	const std::string shadersSaveLocation = "Shaders/";

	const std::string userPrefsSaveLocation = "UserPrefs/";

	const std::string rendererExtension = ".renderer";
	const std::string materialExtension = ".material";
	const std::string textureExtension = ".texture";
	const std::string modelExtension = ".model";
	const std::string levelExtension = ".level";
	const std::string imageExtension = ".png";
	const std::string userPrefsExtension = ".prefs";
	const std::string fragmentExtension = ".frag";
	const std::string vertexExtension = ".vert";
	const std::string computeExtension = ".comp";

	const std::string lastUsedUserPrefsFilePath = "CurrentUserPrefs.info";

}