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

	auto tiles = groundTileParent->transform()->getChildren();
	for (auto i : tiles)
	{
		glm::vec3 pos = i->getGlobalPosition();
		glm::vec2 tileCell = glm::vec2{ pos.x, pos.z } / gridSize;
		tileCell = { roundf(tileCell.x), roundf(tileCell.y) };
		if (!CellAt(tileCell.x - 1, tileCell.y)) {
			PlaceWallAt(pos.x - (gridSize / 2), pos.z, 0.0f);
			// place left wall
		}
		if (!CellAt(tileCell.x, tileCell.y - 1)) {
			// Down wall
		}
		if (!CellAt(tileCell.x + 1, tileCell.y)) {
			// Right
		}
		if (!CellAt(tileCell.x, tileCell.y + 1)) {
			// up
		}
	}
}

SceneObject* LevelEditor::CellAt(float x, float z)
{
	auto tiles = groundTileParent->transform()->getChildren();
	for (auto i : tiles)
	{
		glm::vec3 pos = i->getGlobalPosition();
		glm::vec2 tileCell = glm::vec2{ pos.x, pos.z } / gridSize;
		tileCell = { roundf(tileCell.x), roundf(tileCell.y) };
		if (tileCell.x == x && tileCell.y == z) {
			return i->getSceneObject();
		}
	}
	return nullptr;
}

SceneObject* LevelEditor::PlaceWallAt(float x, float z, float direction)
{
	SceneObject* newWall = new SceneObject(this, "newWall");
	newWall->transform()->setPosition({ x, 0.0f, z });
	newWall->transform()->setEulerRotation({0.0f, direction, 0.0f});
	newWall->transform()->setParent(wallTileParent->transform());
	newWall->setRenderer(new ModelRenderer(wall, (unsigned long long)0));
	// TODO: Add collider
	return newWall;
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
	wall = ResourceManager::LoadModel("models/SM_Wall.fbx");
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
	SceneObject* alreadyPlaced = CellAt(targetCell.x, targetCell.y);

	// TODO: Skip if imgui wants mouse input
	if (ImGui::GetIO().WantCaptureMouse) { return; }
	if (state != BrushState::none && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (!alreadyPlaced) {
			SceneObject* newTile = new SceneObject(this, "tile " + std::to_string(groundTileParent->transform()->getChildren().size()));
			newTile->setRenderer(new ModelRenderer(ground, (unsigned long long)0));
			newTile->transform()->setPosition({ targetCell.x * gridSize, 0.0f, targetCell.y * gridSize });
			newTile->transform()->setParent(groundTileParent->transform());
			
			if (!newTile->transform()->getParent()) {
				std::cout << "\n";
			}
			// other setup here
			gridMinX = (int)fminf(targetCell.x, (float)gridMinX);
			gridMinZ = (int)fminf(targetCell.y, (float)gridMinZ);
			gridMaxX = (int)fmaxf(targetCell.x, (float)gridMaxX);
			gridMaxZ = (int)fmaxf(targetCell.y, (float)gridMaxZ);
		}
	}
	// TODO: Refresh mins and maxes
	if (state != BrushState::none && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && alreadyPlaced) {
		unsigned long long GUID = alreadyPlaced->GUID;
		// TODO: Might be better to have a mark for deletion type of thing and gets deleted afterwards
		// Could get put into some collection somewhere
		delete sceneObjects[GUID];
		sceneObjects.erase(GUID);
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
				break;
			default:
				break;
			}
		}

		if (ImGui::Button("Refresh Walls")) {
			RefreshWalls();
		}
	}
	ImGui::End();
}

LevelEditor::~LevelEditor()
{
}
