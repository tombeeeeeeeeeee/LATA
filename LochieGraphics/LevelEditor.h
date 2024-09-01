#pragma once
#include "Scene.h"
class LevelEditor : public Scene
{
private:

	enum class BrushState {
		none,
		brush
	};
	BrushState state;

	SceneObject* groundTileParent = new SceneObject(this, "Ground Tiles");

	Model* ground;

	glm::vec3 testPos1;
	glm::vec3 testPos2;

	float gridSize = 300.0f;

	int gridMinX = INT_MAX;
	int gridMaxX = INT_MIN;
	int gridMinZ = INT_MAX;
	int gridMaxZ = INT_MIN;


public:

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

	LevelEditor();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
	void GUI() override;
	//	void OnWindowResize() override;
	//
	~LevelEditor() override;

};

