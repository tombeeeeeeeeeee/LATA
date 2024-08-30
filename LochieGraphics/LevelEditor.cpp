#include "LevelEditor.h"

#include "ResourceManager.h"

// TODO: This is only here for the window reference
#include "SceneManager.h"

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

	ground = ResourceManager::LoadModel();
	Mesh* groundMesh = ResourceManager::LoadMesh(Mesh::presets::quad);
	ground->AddMesh(groundMesh);
	camera->transform.setPosition({ 5, 5, 5 });
}

void LevelEditor::Update(float delta)
{
	LineRenderer& lines = renderSystem->lines;
	lines.DrawLineSegment({ 0, 0, 0 }, { 1, 1, 1 }, { 1.0f, 0.5f, 0.2f });
	lines.DrawLineSegment(testPos1, testPos2, {1.0f, 1.0f, 1.0f});
	
	lines.SetColour({ 1, 1, 1 });
	lines.AddPointToLine({ gridSize * gridMinX - 1 - gridSize / 2.0f, 0.0f, gridSize * gridMinZ - 1 - gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMinX - 1 - gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + 1 + gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMaxX + 1 + gridSize / 2.0f, 0.0f, gridSize * gridMaxZ + 1 + gridSize / 2.0f });
	lines.AddPointToLine({ gridSize * gridMaxX + 1 + gridSize / 2.0f, 0.0f, gridSize * gridMinZ - 1 - gridSize / 2.0f});
	lines.FinishLineLoop();

	// TODO: Skip if imgui wants mouse input
	if (ImGui::GetIO().WantCaptureMouse) { return; }
	if (state != BrushState::none && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		std::cout << cursorPos->x << ", " << cursorPos->y << '\n';

		glm::vec3 camPoint = camera->transform.getGlobalPosition();
		camPoint = glm::vec3(camPoint.z, camPoint.y, camPoint.x);

		glm::vec2 adjustedCursor = *cursorPos - glm::vec2{ 0.5f, 0.5f };
		glm::vec3 temp = (camPoint + glm::vec3(adjustedCursor.x * camera->getOrthoWidth(), 0.0f, adjustedCursor.y * camera->getOrthoHeight())) / gridSize;
		
		glm::vec2 targetCell = { roundf(temp.z), roundf(temp.x) };
		
		lines.DrawCircle(glm::vec3{ targetCell.x , 0.0f, targetCell.y } * gridSize, gridSize / 2, {0, 0, 1}, 12);
		
		auto tiles = groundTileParent->transform()->getChildren();
		bool alreadyPlaced = false;
		for (auto i : tiles)
		{
			glm::vec3 pos = i->getGlobalPosition();
			glm::vec2 tileCell = glm::vec2{pos.z, pos.x} / gridSize;
			if (roundf(tileCell.x) == targetCell.x && roundf(tileCell.y) == targetCell.y) {
				alreadyPlaced = true;
			}
		}
		if (!alreadyPlaced) {
			auto newTile = new SceneObject(this, "tile " + std::to_string(tiles.size()));
			newTile->transform()->setPosition({ targetCell.y, 0.0f, targetCell.x });
			newTile->transform()->setParent(groundTileParent->transform());
			// other setup here
			gridMinX = (int)fminf(targetCell.x, (float)gridMinX);
			gridMinZ = (int)fminf(targetCell.y, (float)gridMinZ);
			gridMaxX = (int)fmaxf(targetCell.x, (float)gridMaxX);
			gridMaxZ = (int)fmaxf(targetCell.y, (float)gridMaxZ);
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
			if (state == BrushState::brush) {
				camera->state = Camera::State::tilePlacing;
				camera->transform.setEulerRotation({ 0.0f, 0.0f, -90.0f });
			}
		}
		
		ImGui::End();
	}
}

LevelEditor::~LevelEditor()
{
}
