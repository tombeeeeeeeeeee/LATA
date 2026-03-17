#pragma once
#include "Scene.h"

#include "Lights.h"
#include "Mesh.h"

#include "hashFNV1A.h"

#include <unordered_map>

class GameSyncEcco : public Scene
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

	SceneObject* groundTileParent;

	std::unordered_map<std::pair<int, int>, SceneObject*, hashFNV1A> tiles = {};

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

	bool died = false;
	bool prevDied = false;

	void RefreshMinMaxes();

	void SaveAsPrompt();
	void LoadPrompt();

	void SaveLevel();

	const float timeToShowDeathPicture = 3.0f;
	float timerShowDeathPicture = 0.0f;
	bool showDeathPicture = false;

public:

	bool fadeOut = false;
	float fadeTimer = 0.0f;
	float fadeInTime = 3.5f;
	float fadeOutTime = 1.5f;


	GameSyncEcco();
	void Start() override;
	void Update(float delta) override;
	void AfterDraw(float delta) override;
	void GUI() override;
	~GameSyncEcco() override;
	void Save() override;
	void Load() override;
	void LoadLevel(bool inPlayMaintained = false, std::string levelToLoad = "");
	//static void LoadLevel(bool inPlayMaintained, std::string levelToLoad);
	void InitialiseLayers();
};

