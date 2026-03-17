#include "GameSyncEcco.h"

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
#include "RayAgainstOBB.h"
#include "Collision.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <fstream>
#include <iostream>
#include <filesystem>

void GameSyncEcco::RefreshMinMaxes()
{
	// TODO: prob don't need a seperate min and maxes and could use the other one
	float minX = FLT_MAX;
	float minZ = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxZ = -FLT_MAX;

	gridMinX = INT_MAX;
	gridMinZ = INT_MAX;
	gridMaxX = INT_MIN;
	gridMaxZ = INT_MIN;

	for (auto& i : tiles) {
		glm::vec3 pos = i.second->transform()->getGlobalPosition();
		if (pos.x - gridSize < minX) minX = pos.x - gridSize;
		if (pos.z - gridSize < minZ) minZ = pos.z - gridSize;
		if (pos.x + gridSize > maxX) maxX = pos.x + gridSize;
		if (pos.z + gridSize > maxZ) maxZ = pos.z + gridSize;

		gridMinX = glm::min(gridMinX, i.first.first);
		gridMinZ = glm::min(gridMinZ, i.first.second);

		gridMaxX = glm::max(gridMaxX, i.first.first);
		gridMaxZ = glm::max(gridMaxZ, i.first.second);
	}

	enemySystem.mapMinCorner = { minX, minZ };
	enemySystem.mapDimensions = { maxX - minX, maxZ - minZ };
	enemySystem.mapDimensions /= enemySystem.nfmDensity;
	renderSystem.mapMin = enemySystem.mapMinCorner;
	renderSystem.mapDelta = { maxX - minX, maxZ - minZ };
}

GameSyncEcco::GameSyncEcco() :
	groundTileParent(new SceneObject(this, "Ground Tiles"))
{
}

void GameSyncEcco::Start()
{
	directionalLight.colour = { 0.0f, 0.0f, 0.0f };
	gameUiOverlay = ResourceManager::LoadTexture("images/gameUiOverlay.png", Texture::Type::albedo, GL_CLAMP_TO_EDGE);
	deathScreen = ResourceManager::LoadTexture("images/DeathScreen1.png", Texture::Type::albedo, GL_CLAMP_TO_EDGE);
	overlayMesh.InitialiseQuad(1.0f);
	overlayShader = ResourceManager::LoadShader("Shaders/defaultWithNormal.vert", "Shaders/simpleTexturedWithCutout.frag");

	groundShader = ResourceManager::LoadShader("Shaders/floorWorld.vert", "Shaders/prepass.frag", Shader::Flags::Spec | Shader::Flags::VPmatrix | Shader::Flags::Lit);
	groundTextureAlbedo = ResourceManager::LoadTexture("images/T_MissingTexture.png", Texture::Type::albedo, GL_CLAMP_TO_EDGE);
	groundTexturePBR = ResourceManager::LoadTexture("images/T_MissingTexture.png", Texture::Type::PBR, GL_CLAMP_TO_EDGE);
	groundTexturePBR->mipMapped = false;
	groundTextureAlbedo->mipMapped = false;
	groundMaterial = ResourceManager::LoadMaterial("Ground", groundShader);
	groundMaterial->AddTextures({ groundTextureAlbedo, groundTexturePBR });

	gui.showHierarchy = true;
	gui.showSceneObject = true;
	gui.showCameraMenu = true;

	camera->transform.setPosition({ 0.0f, 1000.0f, 0.0f });
	camera->orthoScale = 300.0f;

	camera->farPlane = 100000.0f;
	camera->nearPlane = 10.0f;

	syncSo = new SceneObject(this, "Sync");
	syncSo->LoadFromPrefab(PrefabManager::loadedPrefabOriginals.at(2091576977596946314ull));
	auto syncChildren = syncSo->transform()->getChildren();
	if (syncChildren.size() > 0) {
		syncAnimatorSo = syncChildren.front()->so;
		if (syncChildren.size() > 1) {
			syncGun = syncChildren.at(1);
		}
		else {
			syncGun = nullptr;
		}
	}
	else {
		syncAnimatorSo = nullptr;
		syncGun = nullptr;
	}

	for (auto& i : syncs)
	{
		i.second.Start(syncAnimatorSo);
	}
	eccoSo = new SceneObject(this, "Ecco");
	eccoSo->LoadFromPrefab(PrefabManager::loadedPrefabOriginals.at(2091576976424546894ull));

	gameCamSystem.cameraPositionDelta = { -150.0f, 100.0f, 150.0f };

	physicsSystem.SetCollisionLayerMask((int)CollisionLayers::sync, (int)CollisionLayers::sync, false);

	if (UserPreferences::loadDefaultLevel && UserPreferences::defaultLevelLoad != "") {
		LoadLevel(false, UserPreferences::defaultLevelLoad);
	}
	if (UserPreferences::defaultCameraSystemLoad != "") {
		std::ifstream file(Paths::systemPath + UserPreferences::defaultCameraSystemLoad + Paths::cameraSystemExtension);
		toml::table data = toml::parse(file);
		gameCamSystem.Load(data);
		gameCamSystem.filename = UserPreferences::defaultCameraSystemLoad;
		file.close();
	}
	if (UserPreferences::defaultEnemySystemLoad != "") {
		std::ifstream file(Paths::systemPath + UserPreferences::defaultEnemySystemLoad + Paths::enemySystemExtension);
		toml::table data = toml::parse(file);
		enemySystem.Load(data);
		enemySystem.filename = UserPreferences::defaultEnemySystemLoad;
		file.close();
	}
	if (UserPreferences::defaultHealthSystemLoad != "") {
		std::ifstream file(Paths::systemPath + UserPreferences::defaultHealthSystemLoad + Paths::healthSystemExtension);
		toml::table data = toml::parse(file);
		healthSystem.Load(data);
		healthSystem.filename = UserPreferences::defaultHealthSystemLoad;
		file.close();
	}

	inPlay = UserPreferences::enterPlayModeOnStart;

	healthBar.InitialiseQuad(1.0f);
	healthShader = ResourceManager::LoadShader("healthBar");
}

void GameSyncEcco::Update(float delta)
{
	directionalLight.colour = { 0.0f, 0.0f, 0.0f };
	bool playerDied = false;

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

		camera->nearPlane = 5000.0f;
		camera->farPlane = 100000.0f;
	}

	else if (lastFramePlayState && !inPlay) //On Play exit
	{
		LoadLevel(false);

		lastFramePlayState = inPlay;
		displayGUI = true;
		enemySystem.aiUpdating = false;

		camera->state = Camera::editorMode;
		triggerSystem.Clear();

		fadeTimer = 0.0f;

		camera->nearPlane = 10.0f;
		camera->farPlane = 50000.0f;

		camera->fov = 70.0f;

		camera->transform.setPosition(gameCamSystem.currentTarget + camera->transform.backward() * 1414.2f);

		renderSystem.exposure = 1.0f;
	}

	lastFramePlayState = inPlay;
	LineRenderer& lines = renderSystem.lines;

	if (input.inputDevices.size() > 0)
	{
		float camera2DForwardLength = glm::length(glm::vec2(camera->transform.forward().x, camera->transform.forward().z));
		float angle = 0.0f;
		if (camera2DForwardLength == 0.0f)
			angle = camera->transform.getEulerRotation().y;
		else
		{
			angle = atan2f(camera->transform.forward().z, camera->transform.forward().x);
			angle *= 180.0f / PI;
			angle += 90.0f;
		}

		if (singlePlayer == 1)
		{
			syncHealPressed = syncSo->sync()->Update(
				syncAnimatorSo,
				*input.inputDevices[0],
				*syncSo->transform(),
				*syncSo->rigidBody(),
				&renderSystem.lines,
				delta,
				angle,
				syncGun,
				died
			);
		}
		else
		{
			eccoHealPressed = eccoSo->ecco()->Update(
				*input.inputDevices[0],
				*eccoSo->transform(),
				*eccoSo->rigidBody(),
				*eccoSo->health(),
				(Directional2dAnimator*)eccoSo->animator(),
				delta,
				angle
			);
		}
		if (singlePlayer == 0)
		{
			if (input.inputDevices.size() > 1)
			{
				syncHealPressed = syncSo->sync()->Update(
					syncAnimatorSo,
					*input.inputDevices[1],
					*syncSo->transform(),
					*syncSo->rigidBody(),
					&renderSystem.lines,
					delta,
					angle,
					syncGun,
					died
				);
			}
		}
	}


	if (inPlay)
	{
		healthSystem.Update(
			healths,
			renderers,
			delta
		);

		for (auto& exitPair : exits)
		{
			if (exitPair.second.Update(this, delta))
				return;
		}

		if (syncHPLastFrame > syncSo->health()->currHealth)
		{
			float randPercentage = std::rand() / (float)RAND_MAX;

			int index = int(floor(randPercentage * 3.0f));
			SceneManager::scene->audio.PlaySound((Audio::SoundIndex)(index + (int)Audio::syncDamageTaken0));
		}
		syncHPLastFrame = syncSo->health()->currHealth;
		// TODO: Remove this here and just change the state when the option is switched via the GUI

		if (singlePlayer == 1) gameCamSystem.target = syncSo->transform()->getGlobalPosition();
		else if (singlePlayer == 2) gameCamSystem.target = eccoSo->transform()->getGlobalPosition();

		gameCamSystem.Update(*camera, *eccoSo->transform(), *syncSo->transform(), camera->orthoScale);

		if (eccoHealPressed && syncHealPressed) healthSystem.PlayerHealingActivate(
			eccoSo->transform()->get2DGlobalPosition(), syncSo->transform()->get2DGlobalPosition(), syncSo->transform(), eccoSo->transform());

		healthSystem.PlayerHealingUpdate(eccoSo->health(), syncSo->health(),
			eccoSo->transform()->get2DGlobalPosition(), syncSo->transform()->get2DGlobalPosition(), delta);

		triggerSystem.Update(plates, triggerables, transforms, delta);
		dabSystem.Update(transforms, doors, bollards, colliders, delta);

		if (!UserPreferences::immortal) {
			if (syncSo->health()->currHealth <= 0 || eccoSo->health()->currHealth <= 0) {
				playerDied = true;
			}
		}
		if (fadeOut) {
			renderSystem.exposure = fmaxf(1 - fadeTimer / fadeOutTime, 0.0f);
		}
		else {
			renderSystem.exposure = fminf(fadeTimer / fadeInTime, 1.0f);
		}
		fadeTimer += delta;
	} // In play
	else { // Not in play
		lines.SetColour({ 1, 1, 1 });
		lines.AddPointToLine({ gridSize * gridMinX - gridSize - gridSize / 2.0f, 0.0f, gridSize * gridMinZ - gridSize - gridSize / 2.0f });
		lines.AddPointToLine({ gridSize * gridMinX - gridSize - gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + gridSize + gridSize / 2.0f });
		lines.AddPointToLine({ gridSize * gridMaxX + gridSize + gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + gridSize + gridSize / 2.0f });
		lines.AddPointToLine({ gridSize * gridMaxX + gridSize + gridSize / 2.0f, 0.0f, gridSize * gridMinZ - gridSize - gridSize / 2.0f });
		lines.FinishLineLoop();
	}


	enemySystem.Update(
		enemies,
		transforms,
		rigidBodies,
		healths,
		spawnManagers,
		animators,
		eccoSo->transform()->get2DGlobalPosition(),
		syncSo->transform()->get2DGlobalPosition(),
		delta
	);

	physicsSystem.UpdateRigidBodies(transforms, rigidBodies, delta);
	if (doCollisions)
		physicsSystem.CollisionCheckPhase(transforms, rigidBodies, colliders);

	prevDied = died;
	if (playerDied) {
		died = true;
	}
	if (!prevDied && died) {
		fadeOut = true;
		fadeTimer = 0.0f;
		timerShowDeathPicture = timeToShowDeathPicture;
	}
	showDeathPicture = false;
	if (died) {
		if (renderSystem.exposure <= 0) {
			timerShowDeathPicture -= delta;
			showDeathPicture = true;
		}
		if (timerShowDeathPicture <= 0) {
			LoadLevel(true);
			renderSystem.exposure = 0.01f;
			fadeOut = false;
			died = false;
			prevDied = false;
		}
	}

	if (glfwGetKey(SceneManager::window, GLFW_KEY_F1))
	{
		LoadLevel(inPlay, "MainMenu");
	}
	else if (glfwGetKey(SceneManager::window, GLFW_KEY_F2))
	{
		LoadLevel(inPlay, "Tutorial");
	}
	else if (glfwGetKey(SceneManager::window, GLFW_KEY_F3))
	{
		LoadLevel(inPlay, "Level_1_WarehouseWorries");
	}
	else if (glfwGetKey(SceneManager::window, GLFW_KEY_F4))
	{
		LoadLevel(inPlay, "Level_2_ObnoxiousOffice");
	}
	else if (glfwGetKey(SceneManager::window, GLFW_KEY_F5))
	{
		LoadLevel(inPlay, "Level_3_DesolateDesks");
	}
	else if (glfwGetKey(SceneManager::window, GLFW_KEY_F6))
	{
		LoadLevel(inPlay, "Level_4_SophisticatedSpaces");
	}
	else if (glfwGetKey(SceneManager::window, GLFW_KEY_F7))
	{
		LoadLevel(inPlay, "Prefabs");
	}
}

void GameSyncEcco::AfterDraw(float delta)
{
	if (windowName == "MainMenu") {
		Health* health = eccoSo->health();
		health->currHealth = health->getMaxHealth();
		return;
	}
	if (inPlay) {

		glDisable(GL_BLEND);
		healthShader->Use();

		Ecco* eccoPart = eccoSo->ecco();
		Sync* syncPart = syncSo->sync();

		eccoPart->boostUI.ApplyToShader(healthShader, eccoPart->getSpeedBoostCooldownPercent());
		healthBar.Draw();
		syncPart->healthUI.ApplyToShader(healthShader, (float)syncSo->health()->currHealth / (float)syncSo->health()->getMaxHealth());
		healthBar.Draw();
		eccoPart->healthUI.ApplyToShader(healthShader, (float)eccoSo->health()->currHealth / (float)eccoSo->health()->getMaxHealth());
		healthBar.Draw();
		syncPart->chargeUI.ApplyToShader(healthShader, syncPart->chargedDuration / syncPart->overclockChargeTime);
		healthBar.Draw();
		healthSystem.abilityUI.ApplyToShader(healthShader, glm::clamp(healthSystem.timeSinceLastHealingAbility / healthSystem.healingAbilityCooldown, 0.0f, 1.0f));
		healthBar.Draw();

		glDepthFunc(GL_ALWAYS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);

		overlayShader->Use();
		gameUiOverlay->Bind(1);
		overlayShader->setFloat("material.alpha", 1.0f);
		overlayShader->setSampler("material.albedo", 1);
		overlayMesh.Draw();

		if (showDeathPicture) {
			float timer = timeToShowDeathPicture - timerShowDeathPicture;
			if (timer <= 0.6f) {
				overlayShader->setFloat("material.alpha", fminf(timer * 2.0f, 1.0f));
			}
			else if (timer > 2.5f) {
				overlayShader->setFloat("material.alpha", timerShowDeathPicture * 2.0f);
			}
			deathScreen->Bind(1);
			overlayMesh.Draw();
		}
	}
}

void GameSyncEcco::GUI()
{
	if (ImGui::Begin("Level Editor")) {

		if (ImGui::Button("PLAY")) {
			inPlay = !inPlay;
		}
		bool multiplayer = singlePlayer == 0;
		bool playAsSync = singlePlayer == 1;
		bool playAsEcco = singlePlayer == 2;

		ImGui::Checkbox("Do Collision Phase", &doCollisions);

		if (ImGui::Checkbox("Play Multiplayer", &multiplayer))
		{
			singlePlayer = 0;
		}
		if (ImGui::Checkbox("Play As Sync", &playAsSync))
		{
			if (playAsSync) {
				singlePlayer = 1;
				camera->state = Camera::targetingPositionPerspective;
			}
			else {
				singlePlayer = 0;
				camera->state = Camera::targetingPlayersPerspective;
			}
		}
		if (ImGui::Checkbox("Play As Ecco", &playAsEcco))
		{
			if (playAsEcco) {
				singlePlayer = 2;
				camera->state = Camera::targetingPositionPerspective;
			}
			else {
				singlePlayer = 0;
				camera->state = Camera::targetingPlayersPerspective;
			}
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
			if (ImGui::MenuItem("Regenerate Enemy Flow Map")) {
				enemySystem.PopulateNormalFlowMap(transforms, rigidBodies, colliders);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void GameSyncEcco::SaveAsPrompt()
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

void GameSyncEcco::LoadPrompt()
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

void GameSyncEcco::SaveLevel()
{

	if (UserPreferences::rememberLastLevel) {
		UserPreferences::defaultLevelLoad = windowName;
		UserPreferences::Save();
	}

	std::ofstream file(Paths::levelsPath + windowName + Paths::levelExtension);

	file << SaveSceneObjectsAndParts();
	file.close();
}

void GameSyncEcco::LoadLevel(bool inPlayMaintained, std::string levelToLoad)
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

	tiles.clear();
	triggerSystem.Clear();

	LoadSceneObjectsAndParts(data);
	lastFramePlayState = false;
	// TODO: this shouldn't need to be here, deleting objects should unselect object
	gui.setSelected(nullptr);

	groundTileParent = FindSceneObjectOfName("Ground Tiles");
	syncSo = FindSceneObjectOfName("Sync");
	auto syncChildren = syncSo->transform()->getChildren();
	if (syncChildren.size() > 0) {
		syncAnimatorSo = syncChildren.front()->so;
		if (syncChildren.size() > 1) {
			syncGun = syncChildren.at(1);
		}
		else {
			syncGun = nullptr;
		}
	}
	else {
		syncAnimatorSo = nullptr;
		syncGun = nullptr;
	}
	eccoSo = FindSceneObjectOfName("Ecco");

	// Refresh the tiles collection
	auto children = groundTileParent->transform()->getChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		glm::vec3 adjustedPos = children[i]->getPosition() / gridSize;
		tiles[{(int)adjustedPos.x, (int)adjustedPos.z}] = children[i]->getSceneObject();
	}
	file.close();

	glm::vec2 min = { FLT_MAX, FLT_MAX };
	glm::vec2 max = { -FLT_MAX, -FLT_MAX };
	for (auto& i : tiles)
	{
		if ((float)i.first.first < min.x) { min.x = (float)i.first.first; };
		if ((float)i.first.second < min.y) { min.y = (float)i.first.second; };

		if ((float)i.first.first > max.x) { max.x = (float)i.first.first; };
		if ((float)i.first.second > max.y) { max.y = (float)i.first.second; };


		ModelRenderer* tileRenderer = i.second->modelRenderer();
		if (tileRenderer) {
			tileRenderer->materials[0] = groundMaterial;
		}
	}
	groundShader->Use();
	groundShader->setVec2("worldMin", (min - glm::vec2(1, 1)) * gridSize);
	groundShader->setVec2("worldMax", (max + glm::vec2(1, 1)) * gridSize);

	InitialiseLayers();

	enemySystem.Start(transforms, rigidBodies, colliders);
	healthSystem.LevelLoad();
	previouslySaved = true;

	// TODO: Move the _Ground to a variable or something
	groundTextureAlbedo->path = Paths::levelsPath + windowName + "_Ground.png";
	groundTextureAlbedo->Load();
	if (!groundTextureAlbedo->loaded) {
		groundTextureAlbedo->path = Paths::levelsPath + windowName + ".png";
		groundTextureAlbedo->Load();
	}

	groundTexturePBR->path = Paths::levelsPath + windowName + "_PBR.tga";
	groundTexturePBR->Load();
	if (!groundTexturePBR->loaded) {
		groundTexturePBR->path = "images/black.png";
		groundTexturePBR->Load();
	}

	RefreshMinMaxes();
	renderSystem.LevelLoad();
	syncSo->transform()->setEulerRotation({ 0.0f, 0.0f, 0.0f });

	for (auto& i : syncs)
	{
		i.second.LevelLoad();
	}
}

void GameSyncEcco::InitialiseLayers()
{
	//ecco
	SceneObject* eccoSO = sceneObjects[eccos.begin()->first];
	eccoSO->rigidBody()->onCollision.push_back([this](Collision collision) { eccos.begin()->second.OnCollision(collision); });
	//sync
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

GameSyncEcco::~GameSyncEcco()
{
}

void GameSyncEcco::Save()
{
	if (!previouslySaved) {
		openSaveAs = true;
	}
	else {
		SaveLevel();
	}
}

void GameSyncEcco::Load()
{
	openLoad = true;
}
