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

	for (int x = gridMinX - 1; x < gridMaxX + 1; x++)
	{
		for (int z = gridMinZ - 1; z < gridMaxZ + 1; z++)
		{
			lines.SetColour({ 1, 1, 1 });
			lines.AddPointToLine({ gridSize * x - gridSize / 2.0f, 0.0f, gridSize * z - gridSize / 2.0f });
			lines.AddPointToLine({ gridSize * x - gridSize / 2.0f, 0.0f, gridSize * z + gridSize / 2.0f });
			lines.AddPointToLine({ gridSize * x + gridSize / 2.0f, 0.0f, gridSize * z + gridSize / 2.0f });
			lines.AddPointToLine({ gridSize * x + gridSize / 2.0f, 0.0f, gridSize * z - gridSize / 2.0f });
			lines.FinishLineLoop();
		}
	}

	// TODO: Skip if imgui wants mouse input
	if (state != BrushState::none && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		std::cout << cursorPos->x << ", " << cursorPos->y << '\n';

		glm::vec3 camPoint = camera->transform.getGlobalPosition();
		camPoint = glm::vec3(camPoint.z, camPoint.y, camPoint.x);

		glm::vec2 adjustedCursor = *cursorPos - glm::vec2{ 0.5f, 0.5f };
		glm::vec3 temp = (camPoint + glm::vec3(adjustedCursor.x * camera->getOrthoWidth(), 0.0f, adjustedCursor.y * camera->getOrthoHeight())) / gridSize;
		lines.DrawCircle(glm::vec3{ roundf(temp.z) , 0.0f, roundf(temp.x)} * gridSize, gridSize / 2, {0, 0, 1}, 12);
		

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
