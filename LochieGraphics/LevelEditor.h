#pragma once
#include "Scene.h"

#include "Input.h"
#include "Lights.h"

#include "hashFNV1A.h"

#include <unordered_map>

class LevelEditor : public Scene
{
private:

	enum class BrushState {
		none,
		brush,
		assetPlacer,
		viewSelect,
	};
	BrushState state = BrushState::none;

	SceneObject* groundTileParent;
	SceneObject* wallTileParent;

	std::unordered_map<std::pair<int, int>, SceneObject*, hashFNV1A> tiles = {};

	Model* ground = nullptr;
	Model* wall = nullptr;

	unsigned int wallCount = 0;
	unsigned int tileCount = 0;

	float gridSize = 300.0f;
	float wallThickness = 25.0f;

	int gridMinX = INT_MAX;
	int gridMaxX = INT_MIN;
	int gridMinZ = INT_MAX;
	int gridMaxZ = INT_MIN;

	bool alwaysRefreshWallsOnPlace = true;
	
	// For save to open the save as prompt, or just save
	bool previouslySaved = false;

	bool openSaveAs = false;
	bool openLoad = false;

	Input input;

	float syncRadius = 10.0f;

	SceneObject* syncSo = nullptr;
	SceneObject* eccoSo = nullptr;

	std::vector<std::string> loadPaths;
	std::vector<std::string*> loadPathsPointers;

	Model* assetPlacer = nullptr;
	float assetPlacerHeight = 0.0f;
	// TODO: Implement
	float assetPlacerRotation = 0.0f;
	float defaultColliderLength = 50.0f;

	float selectSize = 50.0f;

	void RefreshWalls();

	SceneObject* CellAt(float x, float z);

	// Worldspace placing issue
	SceneObject* PlaceWallAt(float x, float z, float direction);
	SceneObject* PlaceTileAt(float x, float z);

	void Brush(glm::vec2 targetCell);
	void Eraser(glm::vec2 targetCell);
	void AssetPlacer(glm::vec2 targetPos);
	void Selector(glm::vec2 targetPos);

	void SaveAsPrompt();
	void LoadPrompt();

	void SaveLevel();

	glm::vec2 EditorCamMouseToWorld() const;

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
	void OnMouseDown() override;
	~LevelEditor() override;
	void Save() override;
	void Load() override;
	void LoadLevel(std::string levelToLoad = "");
};

