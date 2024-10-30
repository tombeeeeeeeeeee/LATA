#pragma once
#include "Scene.h"

#include "Input.h"
#include "Lights.h"

#include "hashFNV1A.h"

#include <unordered_map>

class LevelEditor : public Scene
{
private:


	Shader* groundShader = nullptr;
	Material* groundMaterial = nullptr;
	Texture* groundTexture = nullptr;

	float syncOffsetX = 0.618f;
	float syncOffsetY = -0.845f;
	float syncScaleX = 0.304f;
	float syncScaleY = 0.077f;
	float eccoOffsetX = -0.618f;
	float eccoOffsetY = -0.845f;
	float eccoScaleX = 0.304f;
	float eccoScaleY = 0.077f;

	Mesh healthBar;
	Shader* healthShader;

	enum class BrushState {
		none,
		brush,
		modelPlacer,
		prefabPlacer,
		viewSelect,
	};
	BrushState state = BrushState::none;

	SceneObject* groundTileParent;
	SceneObject* wallTileParent;

	std::unordered_map<std::pair<int, int>, SceneObject*, hashFNV1A> tiles = {};

	Model* ground = nullptr;

	unsigned long long wallSidePrefab = 12423318852501708145;
	unsigned long long wallCornerPrefab = 12472608399756090578;

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

	// TODO: Should be apart of base scene
	Input input;

	float syncRadius = 10.0f;

	SceneObject* syncSo = nullptr;
	SceneObject* eccoSo = nullptr;

	int singlePlayer = 0;

	std::vector<std::string> loadPaths;
	std::vector<std::string*> loadPathsPointers;

	Model* assetPlacer = nullptr;
	float assetPlacerHeight = 0.0f;
	float assetPlacerRotation = 0.0f;
	glm::vec3 assetPlacerColour = { 1.0f, 1.0f, 1.0f };

	float defaultColliderLength = 50.0f;

	bool showGrid = false;
	float placementGridSize = 50.0f;
	float placementGridHeight = 1.0f;
	bool placementGridUseDebugLines = false;
	glm::vec3 gridColour = { 1.0f, 1.0f, 1.0f };
	bool snapToGridEnabled = false;

	float selectSize = 50.0f;

	void RefreshWalls();

	SceneObject* CellAt(float x, float z);

	// Worldspace placing issue
	SceneObject* PlaceWallAt(float x, float z, float direction, unsigned long long prefab);
	SceneObject* PlaceTileAt(float x, float z);

	void Brush(glm::vec2 targetCell);
	void Eraser(glm::vec2 targetCell);
	void ModelPlacer(glm::vec2 targetPos);
	void PrefabPlacer(glm::vec2 targetPos);
	void Selector(glm::vec2 targetPos);
	void DrawGrid();

	void SaveAsPrompt();
	void LoadPrompt();

	void SaveLevel();

	glm::vec2 EditorCamMouseToWorld() const;

public:


	LevelEditor();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
	void GUI() override;
	void OnMouseDown() override;
	~LevelEditor() override;
	void Save() override;
	void Load() override;
	void LoadLevel( bool inPlayMaintained = false, std::string levelToLoad = "" );
};

