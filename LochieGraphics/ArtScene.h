#pragma once
#include "Scene.h"

#include "Texture.h"
#include "Image.h"

#include <unordered_map>
#include <unordered_set>

class ArtScene : public Scene
{
private:

	std::string filter = "";

	std::string artAssetExtension = "Art";

	std::unordered_map<std::string, Texture::Type> acceptableImportTypeNames = {
		{ "BaseColour", Texture::Type::albedo },
		{ "BaseColor",  Texture::Type::albedo },
		{ "Diffuse",    Texture::Type::albedo },
		{ "Albedo",     Texture::Type::albedo},
		{ "Normal",     Texture::Type::normal },
		{ "Roughness",  Texture::Type::roughness},
		{ "Metallic",   Texture::Type::metallic},
		{ "Emissive",   Texture::Type::emission},
		{ "AO",         Texture::Type::ao},
		{ "PBR",        Texture::Type::PBR},
	};

	// Read these from a file or something
	std::string texturePrefix = "T_";
	std::string meshPrefix = "SM_";

	std::string importTextureLocation = "images/";
	std::string importModelLocation = "models/";

	std::string rendererSaveLocation = "Assets/";
	std::string materialSaveLocation = "Assets/";
	std::string textureSaveLocation = "Assets/";
	std::string modelSaveLocation = "Assets/";

	std::string rendererExtension = ".renderer";
	std::string materialExtension = ".material";
	std::string textureExtension = ".texture";
	std::string modelExtension = ".model";

	std::string EnsureCorrectFileLocation(std::string& path, std::string& expected);

	// Images store the image data
	Image roughnessImage;
	Image metallicImage;
	Image aoImage;
	std::unordered_map<std::string, Image*> importImages;

	// Textures for a viewable version, these only exist to see the image in the GUI
	Texture* roughnessPreview = nullptr;
	Texture* metallicPreview = nullptr;
	Texture* aoPreview = nullptr;
	std::unordered_map<std::string, Texture**> importTextures;

	unsigned char missingRoughnessValue = 0;
	unsigned char missingMetallicValue = UCHAR_MAX;
	unsigned char missingAoValue = UCHAR_MAX;

	bool defaultFlip = true;


	void RefreshPBR();

	void RefreshPBRComponents();

	Material* material = nullptr;
	Model* model = nullptr;
	SceneObject* sceneObject = new SceneObject(this, "Editing Scene Object");

	float loadTargetPreviewSize = 128.0f;
	float texturePreviewScale = 0.01f;


	PointLight pointLights[4] = {
		PointLight({ 1.0f, 1.0f, 1.0f }, {  0.0f,  5.2f,  2.0f }, 0.2f, 0.09f, 0.032f, 0),
		PointLight({ 0.8f, 0.8f, 0.8f }, {  2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
		PointLight({ 0.8f, 0.8f, 0.8f }, { -4.0f,  2.0f, -12.f }, 1.0f, 0.09f, 0.032f, 2),
		PointLight({ 0.8f, 0.8f, 0.8f }, {  0.0f,  0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });

	Spotlight spotlight =
		Spotlight({ 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f,  0.0f }, 0.5f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));

	std::vector<std::string> stringPaths;
	std::vector<const char *> newPaths;

	// TODO: Should there be a camera function for this or something similar?
	// Radians
	float resetCamObjectViewSpace = PI / 8;
	void ResetCamera();

	static std::string MaterialNameFromTexturePath(std::string& path);

	void SaveModal();
	bool openSave = false;

	bool saveRenderer = true;
	bool saveModel = true;
	// TODO: Should have some sort of like save container that just has the item and extra info needed for saving
	//struct MaterialSave {
	//	Material* material = nullptr;
	//	bool save = false;
	//};
	//struct TextureSave {
	//	Texture* texture = nullptr;
	//	bool save = false;
	//};

	// TODO: Replace these with vector of a struct for like save info
	std::vector<std::pair<Material*, bool>> materialsToSave;
	// TODO: Doesn't need to save the slot (string), the texture type on the pointer should be enough
	std::vector<std::pair<std::pair<std::string, Texture*>, bool>> texturesToSave;

	void SaveArtAsset();

public:

	static ArtScene* artScene;
	

	static void DragDropCallback(GLFWwindow* window, int pathCount, const char* paths[]);
	void ImportFromPaths(int pathCount, const char* paths[]);
	void ImportTexture(std::string& path, std::string& filename);
	void ImportMesh(std::string& path, std::string& filename);
	void ImportFolder(std::string& path);

	ArtScene();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
	void GUI() override;
//
	~ArtScene() override;

	void Save() override;
	void Load() override;
};