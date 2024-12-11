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
	Texture* groundTextureAlbedo = nullptr;
	Texture* groundTexturePBR = nullptr;

	Shader* overlayShader = nullptr;
	Texture* gameUiOverlay = nullptr;
	Texture* deathScreen = nullptr;
	Mesh overlayMesh;

	Mesh healthBar;
	Shader* healthShader = nullptr;

	enum class BrushState {
		none,
		brush,
		modelPlacer,
		prefabPlacer,
		viewSelect,
	};
	BrushState state = BrushState::none;

	SceneObject* groundTileParent;

	std::unordered_map<std::pair<int, int>, SceneObject*, hashFNV1A> tiles = {};

	Model* ground = nullptr;

	unsigned int tileCount = 0;

	float gridSize = 300.0f;

	int gridMinX = INT_MAX;
	int gridMaxX = INT_MIN;
	int gridMinZ = INT_MAX;
	int gridMaxZ = INT_MIN;

	
	// For save to open the save as prompt, or just save
	bool previouslySaved = false;

	bool openSaveAs = false;
	bool openLoad = false;

	// TODO: Should be apart of base scene
	Input input;

	float syncRadius = 10.0f;

	SceneObject* syncSo = nullptr;
	SceneObject* syncAnimatorSo = nullptr;
	Transform* syncGun = nullptr;
	SceneObject* eccoSo = nullptr;

	int singlePlayer = 0;
	int syncHPLastFrame = 0;

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

	bool multiSelecting = false;
	glm::vec2 multiSelectingPos = { 0.0f, 0.0f };

	bool died = false;
	bool prevDied = false;

	void RefreshMinMaxes();

	SceneObject* CellAt(float x, float z);
	SceneObject* CellAt(int x, int z);

	// Worldspace placing issue
	//SceneObject* PlaceWallAt(float x, float z, float direction, unsigned long long prefab);
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

	const float timeToShowDeathPicture = 3.0f;
	float timerShowDeathPicture = 0.0f;
	bool showDeathPicture = false;

public:

	bool fadeOut = false;
	float fadeTimer = 0.0f;
	float fadeInTime = 3.5f;
	float fadeOutTime = 1.5f;


	LevelEditor();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	void OnMouseDown() override;
	~LevelEditor() override;
	void Save() override;
	void Load() override;
	void LoadLevel( bool inPlayMaintained = false, std::string levelToLoad = "" );
};

