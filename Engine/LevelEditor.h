#pragma once
#include "Scene.h"

#include "Input.h"
#include "Lights.h"
#include "Mesh.h"

#include "hashFNV1A.h"

#include <unordered_map>

class LevelEditor : public Scene
{
private:
	enum class BrushState {
		none,
		modelPlacer,
		prefabPlacer,
		viewSelect,
	};
	BrushState state = BrushState::none;

	float gridSize = 300.0f;

	// For save to open the save as prompt, or just save
	bool previouslySaved = false;

	bool openSaveAs = false;
	bool openLoad = false;

	std::vector<std::string> loadPaths;
	std::vector<std::string*> loadPathsPointers;

	Model* assetPlacer = nullptr;
	float assetPlacerHeight = 0.0f;
	float assetPlacerRotation = 0.0f;
	glm::vec3 assetPlacerColour = { 1.0f, 1.0f, 1.0f };

	float defaultColliderLength = 50.0f;
	
	float selectSize = 50.0f;

	bool multiSelecting = false;
	glm::vec2 multiSelectingPos = { 0.0f, 0.0f };

	void ModelPlacer(glm::vec2 targetPos);
	void PrefabPlacer(glm::vec2 targetPos);
	void Selector(glm::vec2 targetPos);
	
	void SaveAsPrompt();
	void LoadPrompt();

	void SaveLevel();

	glm::vec2 EditorCamMouseToWorld() const;
public:
	LevelEditor();
	void Start() override;
	void Update(float delta) override;
	void GUI() override;
	void OnMouseDown() override;
	~LevelEditor() override;
	void Save() override;
	void Load() override;
	void LoadLevel( bool inPlayMaintained = false, std::string levelToLoad = "" );
	void InitialiseLayers();
};

