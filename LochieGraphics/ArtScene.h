#pragma once
#include "Scene.h"

#include "Image.h"

#include <unordered_map>

class ArtScene : public Scene
{
private:

	std::string filter = "";

	std::vector <std::pair<std::string, const Material*>> filteredMaterials;

	std::unordered_map<std::string, Texture::Type> acceptableImportTypeNames = {
		{ "BaseColour", Texture::Type::albedo },
		{ "Normal",     Texture::Type::normal },
		{ "Albedo",     Texture::Type::albedo},
		{ "Roughness",  Texture::Type::roughness},
		{ "Metallic",   Texture::Type::metallic},
		{ "Emissive",   Texture::Type::emission},
		{ "AO",         Texture::Type::ao},
		//{ "PBR",        Texture::Type::PBR},
	};

	// Read these from a file or something
	std::string texturePrefix = "T_";
	std::string meshPrefix = "SM_";

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

	Texture* pbr = nullptr;

	bool defaultFlip = true;


	void RefreshPBR();

	void RefreshPBRComponents();

	Material* material;
	Model* model;
	SceneObject* sceneObject = new SceneObject(this);

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
//	void OnMouseDown() override;
	void GUI() override;
//	void OnWindowResize() override;
//
	~ArtScene() override;
};