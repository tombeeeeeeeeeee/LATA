#include "LevelEditor.h"

#include "SceneObject.h"
#include "ResourceManager.h"
// TODO: This is only here for the window reference
#include "SceneManager.h"
#include "Paths.h"
#include "Collider.h"
#include "RenderSystem.h"
#include "UserPreferences.h"
#include "PrefabManager.h"
#include "RayAgainstOBB.h"
#include "Collision.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <fstream>
#include <iostream>
#include <filesystem>

LevelEditor::LevelEditor()
{
}

void LevelEditor::Start()
{
	directionalLight.colour = { 0.0f, 0.0f, 0.0f };

	gui.showHierarchy = true;
	gui.showSceneObject = true;
	gui.showCameraMenu = true;

	camera->transform.setPosition({ 0.0f, 1000.0f, 0.0f });
	camera->orthoScale = 300.0f;

	camera->farPlane = 100000.0f;
	camera->nearPlane = 10.0f;

	gameCamSystem.cameraPositionDelta = { -150.0f, 100.0f, 150.0f };

	physicsSystem.SetCollisionLayerMask((int)CollisionLayers::sync, (int)CollisionLayers::sync, false);

	if (UserPreferences::loadDefaultLevel && UserPreferences::defaultLevelLoad != "") {
		LoadLevel(false, UserPreferences::defaultLevelLoad);
	}
	inPlay = UserPreferences::enterPlayModeOnStart;
}

void LevelEditor::Update(float delta)
{
	directionalLight.colour = { 0.0f, 0.0f, 0.0f };
	bool playerDied = false;

	if (multiSelecting && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		multiSelecting = false;
		glm::vec2 mousePos = EditorCamMouseToWorld();
		std::vector<SceneObject*> selected;
		for (auto& i : transforms)
		{
			glm::vec2 pos = i.second.get2DGlobalPosition();
			if (pos.x > fminf(multiSelectingPos.x, mousePos.x) &&
					pos.x < fmaxf(multiSelectingPos.x, mousePos.x) &&
					pos.y > fminf(multiSelectingPos.y, mousePos.y) &&
					pos.y < fmaxf(multiSelectingPos.y, mousePos.y)) {
				selected.push_back(i.second.so);
			}
		}
		gui.setSelected(selected);
	}

	if (!lastFramePlayState && inPlay) //On Play Enter
	{
		if (UserPreferences::saveOnLevelPlay) {
			SaveLevel();
		}

		lastFramePlayState = inPlay;
		displayGUI = false;
		enemySystem.aiUpdating = true;

		enemySystem.SpawnEnemiesInScene(enemies, transforms);
		enemySystem.enemyTags.clear();
		camera->state = Camera::targetingPlayersPerspective;
		dabSystem.Start(transforms, doors);
		triggerSystem.Start(rigidBodies, plates, spawnManagers, doors, bollards, triggerables, spotlights);
		for (auto& pair : exits) pair.second.Initialise(sceneObjects[pair.first]);

		renderSystem.PlayStart(pointLights, spotlights);

		state = BrushState::none;

		camera->nearPlane = 5000.0f;
		camera->farPlane = 100000.0f;
	}

	else if(lastFramePlayState && !inPlay) //On Play exit
	{
		LoadLevel(false);

		lastFramePlayState = inPlay;
		displayGUI = true;
		enemySystem.aiUpdating = false;

		camera->state = Camera::editorMode;
		triggerSystem.Clear();

		camera->nearPlane = 10.0f;
		camera->farPlane = 50000.0f;

		camera->fov = 70.0f;

		camera->transform.setPosition(gameCamSystem.currentTarget + camera->transform.backward() * 1414.2f);

		renderSystem.exposure = 1.0f; 
	}

	lastFramePlayState = inPlay;
	LineRenderer& lines = renderSystem.lines;

	physicsSystem.UpdateRigidBodies(transforms, rigidBodies, delta);
	if(doCollisions)
		physicsSystem.CollisionCheckPhase(transforms, rigidBodies, colliders);

	if (ImGui::GetIO().WantCaptureMouse) { return; }
}

void LevelEditor::GUI()
{
	if (ImGui::Begin("Level Editor")) {

		if (ImGui::Button("PLAY")) {
			inPlay = !inPlay;
		}

		ImGui::Checkbox("Do Collision Phase", &doCollisions);


		if (ImGui::Combo("Brush Mode", (int*)&state, "None\0Brush\0Model Placer\0Prefab Placer\0View Select\0\0")) {
			switch (state)
			{
			case LevelEditor::BrushState::none:
				camera->state = Camera::State::editorMode;
				break;
			case LevelEditor::BrushState::modelPlacer: [[fallthrough]];
			case LevelEditor::BrushState::prefabPlacer: [[fallthrough]];
			case LevelEditor::BrushState::viewSelect:
				camera->state = Camera::State::tilePlacing;
				camera->transform.setEulerRotation({ 90.0f, 180.0f, 0.0f });
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
	}
	ImGui::End();

	SaveAsPrompt();
	LoadPrompt();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save As")) {
				openSaveAs = true;
			}
			if (ImGui::MenuItem("Regenerate Enemy Flow Map")) {
				enemySystem.PopulateNormalFlowMap(transforms, rigidBodies, colliders);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
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
		if (glfwGetKey(SceneManager::window, GLFW_KEY_LEFT_SHIFT)) {
			multiSelecting = true;
			multiSelectingPos = mouseWorld;
		}
		else {
			Selector(mouseWorld);
		}
	}

	if (camera->state == Camera::State::editorMode && state == BrushState::none) {


		glm::vec2 cursorPosNDC = (*cursorPos * 2.0f) - glm::vec2(1.0f, 1.0f);

		glm::vec4 clipPos = glm::inverse(SceneManager::projection) * glm::vec4{ cursorPosNDC.x, cursorPosNDC.y, -1.0f, 1.0f };

		glm::vec3 screenPos = glm::vec3(clipPos) / clipPos.w;
		glm::vec4 worldPosNearPlanevec4 = (glm::inverse(SceneManager::view) * glm::vec4(screenPos, 1.0f));
		glm::vec3 worldPosNearPlane = glm::vec3(worldPosNearPlanevec4);

		glm::vec3 direction = worldPosNearPlane - camera->transform.getGlobalPosition();

		float t = -camera->transform.getGlobalPosition().y / direction.y;

		glm::vec3 clickPosGround = camera->transform.getGlobalPosition() + direction * t;

		//Selector(glm::vec2(clickPosGround.x, clickPosGround.z));

		float shortest = FLT_MAX;
		SceneObject* toSelect = nullptr;
		for (auto& i : sceneObjects)
		{
			if (gui.getSelected() == i.second) { continue; }
			glm::vec3 selectMin;
			glm::vec3 selectMax;
			Model* model = nullptr;
			if (i.second->parts & Parts::modelRenderer) { 
				model = i.second->modelRenderer()->model;
			}
			if (model) {
				selectMin = model->min;
				selectMax = model->max;
			}
			else {
				selectMin = glm::vec3(-selectSize, -selectSize, -selectSize);
				selectMax = glm::vec3(selectSize, selectSize, selectSize);
			}
			float distance = 0.0f;
			if (RayAgainstOBB::RayAgainstOBB(camera->transform.getGlobalPosition(), glm::normalize(direction), selectMin, selectMax, i.second->transform()->getGlobalMatrix(), distance)) {
				Transform* parent = i.second->transform()->getParent();

				if (distance < shortest) {
					toSelect = i.second;
					shortest = distance;
				}
			}
		}
		gui.setSelected(toSelect);
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
	
	if (UserPreferences::rememberLastLevel) {
		UserPreferences::defaultLevelLoad = windowName;
		UserPreferences::Save();
	}

	std::ofstream file(Paths::levelsPath + windowName + Paths::levelExtension);

	file << SaveSceneObjectsAndParts();
	file.close();
}

void LevelEditor::LoadLevel(bool inPlayMaintained, std::string levelToLoad)
{
	if (levelToLoad != "") windowName = levelToLoad;
	std::ifstream file(Paths::levelsPath + windowName + Paths::levelExtension);

	for (auto& i : syncs)
	{
		i.second.currHealth = healths.at(i.first).currHealth;
	}
	for (auto& i : eccos)
	{
		i.second.currHealth = healths.at(i.first).currHealth;
	}
	if (inPlay) inPlay = inPlayMaintained;
	if (!file) {
		std::cout << "Level File not found\n";
		return;
	}

	directionalLight.direction = UserPreferences::loadedDirectionalLightDirection;
	directionalLight.colour = UserPreferences::loadedDirectionalLightColour;

	if (UserPreferences::rememberLastLevel) {
		UserPreferences::defaultLevelLoad = windowName;
		UserPreferences::Save();
	}

	toml::table data = toml::parse(file);

	triggerSystem.Clear();

	LoadSceneObjectsAndParts(data);
	lastFramePlayState = false;
	// TODO: this shouldn't need to be here, deleting objects should unselect object
	gui.setSelected(nullptr);
	file.close();

	glm::vec2 min = { FLT_MAX, FLT_MAX };
	glm::vec2 max = { -FLT_MAX, -FLT_MAX };

	InitialiseLayers();

	enemySystem.Start(transforms, rigidBodies, colliders);
	healthSystem.LevelLoad();
	previouslySaved = true;

	renderSystem.LevelLoad();

	for (auto& i : syncs)
	{
		i.second.LevelLoad();
	}
}

void LevelEditor::InitialiseLayers()
{
	// TODO: Shouldn't be direct sync/ecco references here, there should be an inherited function
////ecco
	SceneObject* eccoSO = sceneObjects[eccos.begin()->first];
	eccoSO->rigidBody()->onCollision.push_back([this](Collision collision) { eccos.begin()->second.OnCollision(collision); });
	////sync
	SceneObject* syncSO = sceneObjects[syncs.begin()->first];
	//if (!syncSO->health()) syncSO->setHealth(new Health());
	syncSO->rigidBody()->vel = { 0.0f, 0.0f };

	for (int i = 1; i < (int)CollisionLayers::count; i *= 2)
	{

		physicsSystem.SetCollisionLayerMask((int)CollisionLayers::reflectiveSurface, i, false);
		physicsSystem.SetCollisionLayerMask((int)CollisionLayers::count, i, false);
		physicsSystem.SetCollisionLayerMask(i, i, false);
		switch (i)
		{
		case (int)CollisionLayers::base:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::trigger, i, false);
			break;
		case (int)CollisionLayers::enemy:
			physicsSystem.SetCollisionLayerMask(i, i, true);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			break;
		case (int)CollisionLayers::trigger:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::enemyProjectile, i, false);
			break;
		case (int)CollisionLayers::enemyProjectile:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			break;
		case (int)CollisionLayers::softCover:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			break;
		default:
			break;
		}
	}
}

void LevelEditor::ModelPlacer(glm::vec2 targetPos)
{
	glm::vec3 pos = { targetPos.x, assetPlacerHeight, targetPos.y };

	SceneObject* newSceneObject = new SceneObject(this, Utilities::FilenameFromPath(assetPlacer->path, false));
	newSceneObject->setModelRenderer(new ModelRenderer(assetPlacer, 0ull));
	newSceneObject->modelRenderer()->setMaterialTint(assetPlacerColour);
	newSceneObject->transform()->setPosition(pos);
	newSceneObject->transform()->setEulerRotation({ 0.0f, assetPlacerRotation, 0.0f });
	newSceneObject->setCollider(new PolygonCollider({
		{ +defaultColliderLength, +defaultColliderLength},
		{ +defaultColliderLength, -defaultColliderLength},
		{ -defaultColliderLength, -defaultColliderLength},
		{ -defaultColliderLength, +defaultColliderLength}
		}, 0.0f));

	gui.setSelected(newSceneObject);
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

	gui.setSelected(newSceneObject);
}

void LevelEditor::Selector(glm::vec2 targetPos)
{
	for (auto& i : transforms)
	{
		Transform* parent = i.second.getParent();
		if (i.second.getSceneObject() == gui.getSelected()) { continue; }
		glm::vec2 pos = i.second.get2DGlobalPosition();
		if (glm::length(pos - targetPos) < selectSize) {
			gui.setSelected(i.second.getSceneObject());
		}
	}
}

glm::vec2 LevelEditor::EditorCamMouseToWorld() const
{
	glm::vec3 camPos = camera->transform.getGlobalPosition();
	glm::vec2 camPoint = glm::vec2(camPos.x, camPos.z);

	glm::vec2 adjustedCursor = *cursorPos - glm::vec2{ 0.5f, 0.5f };
	float scrRatio = (float)*windowWidth / (float)*windowHeight;
	glm::vec2 temp = camPoint + glm::vec2(adjustedCursor.x * camera->getOrthoWidth(scrRatio), -adjustedCursor.y * camera->getOrthoHeight(scrRatio));

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
