#pragma once
#include "Scene.h"

#include <unordered_map>

class ArtScene : public Scene
{
private:

	std::unordered_map<std::string, Texture::Type> acceptableImportTypeNames = {
		{ "BaseColour", Texture::Type::albedo },
		{ "Normal",     Texture::Type::normal },
		{ "Roughness",  Texture::Type::roughness},
		{ "Metallic",   Texture::Type::metallic},
		{ "AO",         Texture::Type::ao},
		{ "Albedo",     Texture::Type::albedo},
	};


	std::string texturePrefix = "T_";
	std::string meshPrefix = "SM_";
	

	Skybox* skybox = nullptr;

	Material* material;
	Model model;
	SceneObject* sceneObject = new SceneObject(this);


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


public:

	static ArtScene* artScene;

	static void DragDropCallback(GLFWwindow* window, int pathCount, const char* paths[]);
	void ImportFromPaths(int pathCount, const char* paths[]);
	void ImportTexture(std::string& path, std::string& filename);
	void ImportMesh(std::string& path, std::string& filename);

	ArtScene();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
//	void OnMouseDown() override;
	void GUI() override;
//	void UpdateAllTransforms();
//	void OnWindowResize() override;
//
	~ArtScene() override;
};