#pragma once
#include "Scene.h"

#include <unordered_map>

class ArtScene : public Scene
{
private:

	std::unordered_map<std::string, Texture::Type> acceptableImportTypeNames = {
		{ "BaseColour", Texture::Type::albedo },
		{ "Normal",     Texture::Type::normal },
		{ "Albedo",     Texture::Type::albedo},
		{ "Roughness",  Texture::Type::roughness},
		{ "Metallic",   Texture::Type::metallic},
		{ "AO",         Texture::Type::ao},
		//{ "PBR",        Texture::Type::PBR},
	};


	std::string texturePrefix = "T_";
	std::string meshPrefix = "SM_";

	Texture* base = nullptr;
	Texture* roughness = nullptr;
	Texture* normal = nullptr;
	Texture* metallic = nullptr;
	Texture* ao = nullptr;

	Texture* pbr = nullptr;

	bool defaultFlip = false;

	std::unordered_map<std::string, Texture**> importTextures;

	void RefreshPBR();
	
	GLint defaultUIShader;
	static Shader* singleChannelUIImage;
	static void SetSingleChannelUIShader(const ImDrawList* parent_list, const ImDrawCmd* cmd);
	static void SetToDefaultUIShader(const ImDrawList* parent_list, const ImDrawCmd* cmd);

	Skybox* skybox = nullptr;

	Material* material;
	Model* model;
	SceneObject* sceneObject = new SceneObject(this);

	float loadTargetPreviewSize = 128.0f;
	float texturePreviewScale = 1.0f;


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