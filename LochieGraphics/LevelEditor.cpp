#include "LevelEditor.h"

#include "ResourceManager.h"

// TODO: This is only here for the window reference
#include "SceneManager.h"

void LevelEditor::RefreshWalls()
{
	auto walls = wallTileParent->transform()->getChildren();
	
	while (walls.size())
	{
		unsigned long long GUID = walls.front()->getSceneObject()->GUID;
		delete sceneObjects[GUID];
		sceneObjects.erase(GUID);
		walls.erase(walls.begin());
	}
	wallCount = 0;


	float offset = (gridSize + wallThickness) / 2;
	for (auto& i : tiles)
	{
		glm::vec3 pos = i.second->transform()->getGlobalPosition();
		glm::vec2 tileCell = glm::vec2{ pos.x, pos.z } / gridSize;
		tileCell = { roundf(tileCell.x), roundf(tileCell.y) };
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
}

SceneObject* LevelEditor::CellAt(float x, float z)
{
	auto tile = tiles.find({(int)x, (int)z});
	if (tile == tiles.end()) { return nullptr; }
	else { return tile->second; }
}

SceneObject* LevelEditor::PlaceWallAt(float x, float z, float direction)
{
	SceneObject* newWall = new SceneObject(this, "newWall" + std::to_string(++wallCount));
	newWall->transform()->setPosition({ x, 0.0f, z });
	newWall->transform()->setEulerRotation({0.0f, direction, 0.0f});
	newWall->transform()->setParent(wallTileParent->transform());
	newWall->setRenderer(new ModelRenderer(wall, (unsigned long long)0));
	// TODO: Add collider
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

LevelEditor::LevelEditor()
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

	camera->transform.setPosition({ 0, 50, 0 });
	camera->orthoScale = 300;
	
	camera->editorSpeed.move = 300;
	camera->farPlane = 100000;
	camera->nearPlane = 10;

	ground = ResourceManager::LoadModel("models/SM_FloorTile.fbx");
	wall = ResourceManager::LoadModel("models/adjustedOriginLocWall.fbx");
}

void LevelEditor::Update(float delta)
{
	LineRenderer& lines = renderSystem->lines;

	lines.DrawLineSegment({ 0, 0, 0 }, { 1, 1, 1 }, { 1.0f, 0.5f, 0.2f });
	lines.DrawLineSegment(testPos1, testPos2, {1.0f, 1.0f, 1.0f});
	
	lines.SetColour({ 1, 1, 1 });
	lines.AddPointToLine({ gridSize * gridMinX - gridSize - gridSize / 2.0f, 0.0f, gridSize * gridMinZ - gridSize - gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMinX - gridSize - gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + gridSize + gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMaxX + gridSize + gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + gridSize + gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMaxX + gridSize + gridSize / 2.0f, 0.0f, gridSize * gridMinZ - gridSize - gridSize / 2.0f});
	lines.FinishLineLoop();

	glm::vec3 camPoint = camera->transform.getGlobalPosition();
	camPoint = glm::vec3(camPoint.x, camPoint.y, camPoint.z);

	glm::vec2 adjustedCursor = *cursorPos - glm::vec2{ 0.5f, 0.5f };
	glm::vec3 temp = (camPoint + glm::vec3(adjustedCursor.x * camera->getOrthoWidth(), 0.0f, -adjustedCursor.y * camera->getOrthoHeight())) / gridSize;
	glm::vec2 targetCell = glm::vec2{ roundf(temp.x), roundf(temp.z) };

	// TODO: Skip if imgui wants mouse input
	if (ImGui::GetIO().WantCaptureMouse) { return; }
	if (state != BrushState::none && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		SceneObject* alreadyPlaced = CellAt(targetCell.x, targetCell.y);
		if (!alreadyPlaced) {
			PlaceTileAt(targetCell.x, targetCell.y);
			// other setup here
			gridMinX = (int)fminf(targetCell.x, (float)gridMinX);
			gridMinZ = (int)fminf(targetCell.y, (float)gridMinZ);
			gridMaxX = (int)fmaxf(targetCell.x, (float)gridMaxX);
			gridMaxZ = (int)fmaxf(targetCell.y, (float)gridMaxZ);

			if (alwaysRefreshWallsOnPlace) { RefreshWalls(); }
		}
	}
	// TODO: Refresh mins and maxes
	if (state != BrushState::none && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		SceneObject* alreadyPlaced = CellAt(targetCell.x, targetCell.y);
		if (alreadyPlaced) {
			unsigned long long GUID = alreadyPlaced->GUID;
			// TODO: Might be better to have a mark for deletion type of thing and gets deleted afterwards
			// Could get put into some collection somewhere
			delete sceneObjects[GUID];
			sceneObjects.erase(GUID);
			tiles.erase({ (int)targetCell.x, (int)targetCell.y });

			if (alwaysRefreshWallsOnPlace) { RefreshWalls(); }
		}
	}
}

void LevelEditor::Draw()
{
	renderSystem->Update(
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
		if (ImGui::Combo("Brush Mode", (int*)&state, "None\0Brush\0\0")) {
			switch (state)
			{
			case LevelEditor::BrushState::none:
				camera->state = Camera::State::editorMode;
				break;
			case LevelEditor::BrushState::brush:
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
}

LevelEditor::~LevelEditor()
{
}
