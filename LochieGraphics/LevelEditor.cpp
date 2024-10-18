#include "LevelEditor.h"

#include "SceneObject.h"
#include "ResourceManager.h"
// TODO: This is only here for the window reference
#include "SceneManager.h"
#include "Paths.h"
#include "Collider.h"
#include "Sync.h"
#include "Ecco.h"
#include "RenderSystem.h"
#include "UserPreferences.h"
#include "PrefabManager.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <fstream>
#include <iostream>
#include <filesystem>

void LevelEditor::RefreshWalls()
{
	auto walls = wallTileParent->transform()->getChildren();

	while (walls.size())
	{
		// TODO: Should be using a delete sceneobject function
		unsigned long long GUID = walls.front()->getSceneObject()->GUID;
		
		delete sceneObjects[GUID];
		sceneObjects.erase(GUID);
		
		walls.erase(walls.begin());
	}
	wallCount = 0;

	float offset = (gridSize + wallThickness) / 2;
	float minX = FLT_MAX;
	float minZ = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxZ = -FLT_MAX;
	for (auto& i : tiles)
	{
		glm::vec3 pos = i.second->transform()->getGlobalPosition();
		glm::vec2 tileCell = glm::vec2{ pos.x, pos.z } / gridSize;
		tileCell = { roundf(tileCell.x), roundf(tileCell.y) };

		if (pos.x - gridSize < minX) minX = pos.x - gridSize;
		if (pos.z - gridSize < minZ) minZ = pos.z - gridSize;
		if (pos.x + gridSize > maxX) maxX = pos.x + gridSize;
		if (pos.z + gridSize > maxZ) maxZ = pos.z + gridSize;

		if (!CellAt(tileCell.x - 1, tileCell.y)) {
			PlaceWallAt(pos.x - offset, pos.z, 0.0f);
		}
		if (!CellAt(tileCell.x, tileCell.y - 1)) {
			PlaceWallAt(pos.x, pos.z - offset, 90.0f);
		}
		if (!CellAt(tileCell.x + 1, tileCell.y)) {
			PlaceWallAt(pos.x + offset, pos.z, 0.0f);
		}
		if (!CellAt(tileCell.x, tileCell.y + 1)) {
			PlaceWallAt(pos.x, pos.z + offset, 90.0f);
		}
	}

	enemySystem.mapMinCorner = {minX, minZ};
	enemySystem.mapDimensions = {maxX - minX, maxZ - minZ};
	enemySystem.mapDimensions /= enemySystem.nfmDensity;
}

SceneObject* LevelEditor::CellAt(float x, float z)
{
	auto tile = tiles.find({ (int)x, (int)z });
	if (tile == tiles.end()) { return nullptr; }
	else { return tile->second; }
}

SceneObject* LevelEditor::PlaceWallAt(float x, float z, float direction)
{
	SceneObject* newWall = new SceneObject(this, "newWall" + std::to_string(++wallCount));
	newWall->transform()->setPosition({ x, 0.0f, z });
	newWall->transform()->setEulerRotation({ 0.0f, direction, 0.0f });
	newWall->transform()->setParent(wallTileParent->transform());
	newWall->setRenderer(new ModelRenderer(wall, (unsigned long long)0));
	// TODO: Make sure there isn't memory leaks
	RigidBody* newRigidBody = new RigidBody(0.0f, 0.25f, {}, true);
	newWall->setRigidBody(newRigidBody);
	newRigidBody = newWall->rigidbody();
	float wallLength = 150.f;
	float wallThickness = 12.5f;
	newRigidBody->addCollider(new PolygonCollider({
		{ +wallThickness,  +wallLength}, { +wallThickness,  -wallLength}, {  -wallThickness,  -wallLength }, { -wallThickness,  +wallLength}
		}, 0.0f));

	return newWall;
}

SceneObject* LevelEditor::PlaceTileAt(float x, float z)
{
	SceneObject* newTile = new SceneObject(this, "tile " + std::to_string(++tileCount));
	newTile->setRenderer(new ModelRenderer(ground, (unsigned long long)0));
	newTile->transform()->setPosition({ x * gridSize, 0.0f, z * gridSize });
	newTile->transform()->setParent(groundTileParent->transform());
	tiles[{(int)x, (int)z}] = newTile;
	return newTile;
}

void LevelEditor::Brush(glm::vec2 targetCell)
{
	SceneObject* alreadyPlaced = CellAt(targetCell.x, targetCell.y);
	if (alreadyPlaced) {
		return;
	}
	PlaceTileAt(targetCell.x, targetCell.y);
	// other setup here
	gridMinX = (int)fminf(targetCell.x, (float)gridMinX);
	gridMinZ = (int)fminf(targetCell.y, (float)gridMinZ);

	gridMaxX = (int)fmaxf(targetCell.x, (float)gridMaxX);
	gridMaxZ = (int)fmaxf(targetCell.y, (float)gridMaxZ);

	if (alwaysRefreshWallsOnPlace) { RefreshWalls(); }
}

void LevelEditor::Eraser(glm::vec2 targetCell)
{
	SceneObject* alreadyPlaced = CellAt(targetCell.x, targetCell.y);
	if (!alreadyPlaced) {
		return;
	}
	DeleteSceneObject(alreadyPlaced->GUID);
	tiles.erase({ (int)targetCell.x, (int)targetCell.y });

	if (alwaysRefreshWallsOnPlace) { RefreshWalls(); }
}

LevelEditor::LevelEditor() :
	groundTileParent(new SceneObject(this, "Ground Tiles")),
	wallTileParent(new SceneObject(this, "Wall Tiles"))
{
}

void LevelEditor::Start()
{
	lights.insert(lights.end(), {
	&directionalLight,
	&spotlight,
	&pointLights[0],
	&pointLights[1],
	&pointLights[2],
	&pointLights[3],
		});

	gui.showHierarchy = true;
	gui.showSceneObject = true;
	gui.showCameraMenu = true;

	camera->transform.setPosition({ 0.0f, 50.0f, 0.0f });
	camera->orthoScale = 300.0f;

	camera->editorSpeed.move = 300.0f;
	camera->farPlane = 100000.0f;
	camera->nearPlane = 10.0f;

	ground = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_FloorTile" + Paths::modelExtension);
	wall = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_adjustedOriginLocWall" + Paths::modelExtension);

	syncSo = new SceneObject(this, "Sync");
	eccoSo = new SceneObject(this, "Ecco");
	sync->GUID = syncSo->GUID;
	ecco->GUID = eccoSo->GUID;


	RigidBody* hRb = new RigidBody();
	hRb->setMass(1.0f);
	hRb->addCollider({ new PolygonCollider({{0.0f, 0.0f}}, syncRadius, CollisionLayers::sync) });
	hRb->setMomentOfInertia(5.0f);

	RigidBody* rRb = new RigidBody();
	float eccoColliderSize = 75.0f;
	rRb->addCollider({ new PolygonCollider(
		{
			{eccoColliderSize, eccoColliderSize},
			{eccoColliderSize, -eccoColliderSize},
			{-eccoColliderSize, -eccoColliderSize},
			{-eccoColliderSize, eccoColliderSize},
		}, 0.0f, CollisionLayers::ecco) }
	);
	rRb->setMass(0.1f);
	rRb->setMomentOfInertia(5.0f);

	syncSo->transform()->setPosition({ 1.0f,0.0f,1.0f });

	input.Initialise();

	syncSo->setRigidBody(hRb);
	eccoSo->setRigidBody(rRb);

	hRb = &rigidBodies[sync->GUID];
	rRb = &rigidBodies[ecco->GUID];
	syncSo->setSync(sync);
	eccoSo->setEcco(ecco);
	ecco->wheelDirection = { eccoSo->transform()->forward().x, eccoSo->transform()->forward().y };

	gameCamSystem.cameraPositionDelta = { -150.0f, 100.0f, 150.0f };

	eccoSo->setRenderer(new ModelRenderer(ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_EccoRotated" + Paths::modelExtension), (unsigned long long)0));
	camera->transform.setRotation(glm::quat(0.899f, -0.086f, 0.377f, -0.205f));

	sync->Start(&shaders);

	healthSystem.Start(healths);

	physicsSystem.SetCollisionLayerMask((int)CollisionLayers::sync, (int)CollisionLayers::sync, false);

	for (auto& i : std::filesystem::directory_iterator(Paths::modelSaveLocation))
	{
		ResourceManager::LoadModelAsset(i.path().string());
	}

	renderSystem.ssaoRadius = 64.0f;
	renderSystem.ssaoBias = 32.0f;

	if (UserPreferences::loadDefaultLevel && UserPreferences::defaultLevelLoad != "") {
		LoadLevel(false, UserPreferences::defaultLevelLoad);
	}

	inPlay = UserPreferences::enterPlayModeOnStart;
}

void LevelEditor::Update(float delta)
{
	if (!lastFramePlayState && inPlay) //On Play Enter
	{
		SaveLevel();

		lastFramePlayState = inPlay;
		displayGUI = false;
		enemySystem.aiUpdating = true;

		enemySystem.SpawnEnemiesInScene(enemies, transforms);

		camera->state = Camera::targetingPlayers;
	}
	else if(lastFramePlayState && !inPlay) //On Play exit
	{
		LoadLevel(false);

		lastFramePlayState = inPlay;
		displayGUI = true;
		enemySystem.aiUpdating = false;

		camera->state = Camera::editorMode;
	}

	lastFramePlayState = inPlay;
	LineRenderer& lines = renderSystem.lines;
	input.Update();

	physicsSystem.UpdateRigidBodies(transforms, rigidBodies, delta);
	physicsSystem.CollisionCheckPhase(transforms, rigidBodies, colliders);


	if (inPlay)
	{
		healthSystem.Update(
			healths,
			renderers,
			delta
		);

		for (auto& exitPair : exits) exitPair.second.Update();

		gameCamSystem.Update(*camera, *eccoSo->transform(), *syncSo->transform(), camera->orthoScale);

		if (eccoHealPressed && syncHealPressed) healthSystem.PlayerHealingActivate(
			eccoSo->transform()->get2DGlobalPosition(), syncSo->transform()->get2DGlobalPosition());

		healthSystem.PlayerHealingUpdate(eccoSo->health(), syncSo->health(),
			eccoSo->transform()->get2DGlobalPosition(), syncSo->transform()->get2DGlobalPosition(), delta);

	}

	enemySystem.Update(
		enemies,
		transforms,
		rigidBodies,
		healths,
		eccoSo->transform()->get2DGlobalPosition(),
		syncSo->transform()->get2DGlobalPosition(),
		delta
	);

	if (input.inputDevices.size() > 0)
	{
		float camera2DForwardLength = glm::length(glm::vec2( camera->transform.forward().x, camera->transform.forward().z ));
		float angle = 0.0f;
		if (camera2DForwardLength == 0.0f)
			angle = camera->transform.getEulerRotation().y;
		else
		{
			angle = atan2f(camera->transform.forward().z, camera->transform.forward().x);
			angle *= 180.0f / PI;
			angle += 90.0f;
		}

		eccoHealPressed = ecco->Update(
			*input.inputDevices[0],
			*eccoSo->transform(),
			*eccoSo->rigidbody(),
			*eccoSo->health(),
			delta,
			angle
		);

		if (input.inputDevices.size() > 1)
		{
			syncHealPressed = sync->Update(
				*input.inputDevices[1],
				*syncSo->transform(),
				*syncSo->rigidbody(),
				&renderSystem.lines,
				delta,
				angle
			);
		}
	}


	lines.SetColour({ 1, 1, 1 });
	lines.AddPointToLine({ gridSize * gridMinX - gridSize - gridSize / 2.0f, 0.0f, gridSize * gridMinZ - gridSize - gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMinX - gridSize - gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + gridSize + gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMaxX + gridSize + gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + gridSize + gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMaxX + gridSize + gridSize / 2.0f, 0.0f, gridSize * gridMinZ - gridSize - gridSize / 2.0f });
	lines.FinishLineLoop();

	glm::vec2 targetCell = EditorCamMouseToWorld() / gridSize;
	targetCell = glm::vec2{ roundf(targetCell.x), roundf(targetCell.y) };

	if (ImGui::GetIO().WantCaptureMouse) { return; }
	if (state == BrushState::brush && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		Brush(targetCell);
	}
	// TODO: Refresh mins and maxes
	if (state == BrushState::brush && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		Eraser(targetCell);
	}

}

void LevelEditor::Draw()
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);
}

void LevelEditor::GUI()
{
	if (ImGui::Begin("Level Editor")) {

		if (ImGui::Button("PLAY")) {
			inPlay = !inPlay;
		}

		if (ImGui::Combo("Brush Mode", (int*)&state, "None\0Brush\0Model Placer\0Prefab Placer\0View Select\0\0")) {
			switch (state)
			{
			case LevelEditor::BrushState::none:
				camera->state = Camera::State::editorMode;
				break;
			case LevelEditor::BrushState::brush: [[fallthrough]];
			case LevelEditor::BrushState::modelPlacer: [[fallthrough]];
			case LevelEditor::BrushState::prefabPlacer: [[fallthrough]];
			case LevelEditor::BrushState::viewSelect:
				camera->state = Camera::State::tilePlacing;
				camera->transform.setEulerRotation({ -90.0f, 0.0f, -90.0f });
				glm::vec3 pos = camera->transform.getPosition();
				pos.y = fmaxf(pos.y, 600);
				camera->transform.setPosition(pos);
				break;
			default:
				break;
			}
		}

		ImGui::Indent();
		if (state == BrushState::modelPlacer) {
			ResourceManager::ModelSelector("Asset To Place", &assetPlacer);
			ImGui::ColorEdit3("Asset Placement Colour", &assetPlacerColour.x);
		}
		if (state == BrushState::prefabPlacer) {
			PrefabManager::PrefabSelector();
		}
		if (state == BrushState::modelPlacer || state == BrushState::prefabPlacer) {
			ImGui::DragFloat("Placement Height", &assetPlacerHeight);
			ImGui::DragFloat("Placement Rotation", &assetPlacerRotation);
		}
		ImGui::Unindent();

		ImGui::Checkbox("Always refresh walls", &alwaysRefreshWallsOnPlace);
		if (!alwaysRefreshWallsOnPlace) {
			if (ImGui::Button("Refresh Walls")) {
				RefreshWalls();
			}
		}
		if (ImGui::DragFloat("Wall offset", &wallThickness, 0.5f)) {
			if (alwaysRefreshWallsOnPlace) { RefreshWalls(); }
		}
	}
	ImGui::End();

	SaveAsPrompt();
	LoadPrompt();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save As")) {
				openSaveAs = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	input.GUI();
}

void LevelEditor::OnMouseDown()
{
	if (state == BrushState::modelPlacer && assetPlacer != nullptr) {
		glm::vec2 mouseWorld = EditorCamMouseToWorld();
		ModelPlacer(mouseWorld);
	}

	if (state == BrushState::prefabPlacer) {
		glm::vec2 mouseWorld = EditorCamMouseToWorld();
		PrefabPlacer(mouseWorld);
	}

	if (camera->state == Camera::State::tilePlacing && state == BrushState::viewSelect) {
		glm::vec2 mouseWorld = EditorCamMouseToWorld();
		Selector(mouseWorld);
	}
}

void LevelEditor::SaveAsPrompt()
{
	if (openSaveAs) {
		ImGui::OpenPopup("Save as");
	}
	if (!ImGui::BeginPopupModal("Save as", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		return;
	}

	if (openSaveAs) {
		ImGui::SetKeyboardFocusHere();
		openSaveAs = false;
	}
	if (ImGui::InputText("Filename##Save", &windowName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
		ImGui::CloseCurrentPopup();
		previouslySaved = true;
		SaveLevel();
	}
	if (ImGui::Button("Save")) {
		ImGui::CloseCurrentPopup();
		previouslySaved = true;
		SaveLevel();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##Load")) {
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void LevelEditor::LoadPrompt()
{
	if (openLoad) {
		ImGui::OpenPopup("Load Level", ImGuiPopupFlags_AnyPopup);
	}
	if (!ImGui::BeginPopupModal("Load Level", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		return;
	}

	if (openLoad) {
		openLoad = false;

		loadPaths.clear();
		loadPathsPointers.clear();

		for (auto& i : std::filesystem::directory_iterator(Paths::levelsPath))
		{
			loadPaths.push_back(i.path().generic_string().substr(Paths::levelsPath.size()));
			if (loadPaths.back().substr(loadPaths.back().size() - Paths::levelExtension.size()) != Paths::levelExtension) {
				loadPaths.erase(--loadPaths.end());
				continue;
			}
			loadPaths.back() = loadPaths.back().substr(0, loadPaths.back().size() - Paths::levelExtension.size());
		}
		for (auto& i : loadPaths)
		{
			loadPathsPointers.push_back(&i);
		}
	}

	std::string* selected = &windowName;
	if (ExtraEditorGUI::InputSearchBox(loadPathsPointers.begin(), loadPathsPointers.end(), &selected, "Filename", Utilities::PointerToString(&loadPathsPointers), false)) {
		ImGui::CloseCurrentPopup();
		windowName = *selected;
		LoadLevel();
	}

	if (ImGui::Button("Load##Load")) {
		ImGui::CloseCurrentPopup();
		LoadLevel();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##Load")) {
		ImGui::CloseCurrentPopup();
	}


	ImGui::EndPopup();
}

void LevelEditor::SaveLevel()
{
	enemySystem.PopulateNormalFlowMapFromRigidBodies(transforms, rigidBodies, colliders);
	
	if (UserPreferences::rememberLastLevel) {
		UserPreferences::defaultLevelLoad = windowName;
	}

	std::ofstream file(Paths::levelsPath + windowName + Paths::levelExtension);

	file << SaveSceneObjectsAndParts();
	file.close();
}

void LevelEditor::LoadLevel(bool inPlayMaintained, std::string levelToLoad)
{
	if (levelToLoad != "") windowName = levelToLoad;
	std::ifstream file(Paths::levelsPath + windowName + Paths::levelExtension);

	ecco->currHealth = healths[ecco->GUID].currHealth;
	sync->currHealth = healths[sync->GUID].currHealth;
	if (inPlay) inPlay = inPlayMaintained;
	if (!file) {
		std::cout << "Level File not found\n";
		return;
	}

	if (UserPreferences::rememberLastLevel) {
		UserPreferences::defaultLevelLoad = windowName;
	}

	toml::table data = toml::parse(file);

	DeleteAllSceneObjects();

	LoadSceneObjectsAndParts(data);

	gui.sceneObjectSelected = nullptr;

	groundTileParent = FindSceneObjectOfName("Ground Tiles");
	wallTileParent = FindSceneObjectOfName("Wall Tiles");
	syncSo = FindSceneObjectOfName("Sync");
	eccoSo = FindSceneObjectOfName("Ecco");


	// Refresh the tiles collection
	tiles.clear();
	auto children = groundTileParent->transform()->getChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		glm::vec3 adjustedPos = children[i]->getPosition() / gridSize;
		tiles[{(int)adjustedPos.x, (int)adjustedPos.z}] = children[i]->getSceneObject();
	}
	file.close();

	RefreshWalls();
	InitialisePlayers();

	enemySystem.Start(transforms, rigidBodies, colliders);
	previouslySaved = true;
}

void LevelEditor::ModelPlacer(glm::vec2 targetPos)
{
	glm::vec3 pos = { targetPos.x, assetPlacerHeight, targetPos.y };

	SceneObject* newSceneObject = new SceneObject(this, Utilities::FilenameFromPath(assetPlacer->path, false));
	newSceneObject->setRenderer(new ModelRenderer(assetPlacer, 0ull));
	newSceneObject->renderer()->setMaterialTint(assetPlacerColour);
	newSceneObject->transform()->setPosition(pos);
	newSceneObject->transform()->setEulerRotation({ 0.0f, assetPlacerRotation, 0.0f });
	newSceneObject->setCollider(new PolygonCollider({
		{ +defaultColliderLength, +defaultColliderLength},
		{ +defaultColliderLength, -defaultColliderLength},
		{ -defaultColliderLength, -defaultColliderLength},
		{ -defaultColliderLength, +defaultColliderLength}
		}, 0.0f));

	gui.sceneObjectSelected = newSceneObject;
}

void LevelEditor::PrefabPlacer(glm::vec2 targetPos)
{
	if (PrefabManager::loadedPrefabOriginals.find(PrefabManager::selectedPrefab) == PrefabManager::loadedPrefabOriginals.end()) {
		// No prefab selected
		return;
	}
	glm::vec3 pos = { targetPos.x, assetPlacerHeight, targetPos.y };

	SceneObject* newSceneObject = new SceneObject(this);

	newSceneObject->LoadFromPrefab(PrefabManager::loadedPrefabOriginals.at(PrefabManager::selectedPrefab));

	newSceneObject->transform()->setPosition(pos);
	newSceneObject->transform()->setEulerRotation({ 0.0f, assetPlacerRotation, 0.0f });

	gui.sceneObjectSelected = newSceneObject;
}

void LevelEditor::Selector(glm::vec2 targetPos)
{
	for (auto& i : transforms)
	{
		Transform* parent = i.second.getParent();
		if (parent) {
			if (parent->getSceneObject() == wallTileParent || parent->getSceneObject() == groundTileParent) {
				continue;
			}
		}
		if (i.second.getSceneObject() == gui.sceneObjectSelected) { continue; }
		glm::vec3 temp = i.second.getPosition();
		glm::vec2 pos = { temp.x, temp.z };
		if (glm::length(pos - targetPos) < selectSize) {
			gui.sceneObjectSelected = i.second.getSceneObject();
		}
	}
}

glm::vec2 LevelEditor::EditorCamMouseToWorld() const
{
	glm::vec3 camPos = camera->transform.getGlobalPosition();
	glm::vec2 camPoint = glm::vec2(camPos.x, camPos.z);

	glm::vec2 adjustedCursor = *cursorPos - glm::vec2{ 0.5f, 0.5f };
	glm::vec2 temp = camPoint + glm::vec2(adjustedCursor.x * camera->getOrthoWidth(), -adjustedCursor.y * camera->getOrthoHeight());

	return temp;
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Save()
{
	if (!previouslySaved) {
		openSaveAs = true;
	}
	else {
		SaveLevel();
	}
}

void LevelEditor::Load()
{
	openLoad = true;
}
