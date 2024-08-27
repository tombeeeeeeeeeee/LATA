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
}

void LevelEditor::Update(float delta)
{
	// TODO: Skip if imgui wants mouse input

	if (camera->InOrthoMode()) {
		if (glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			std::cout << cursorPos->x << ", " << cursorPos->y << '\n';

			glm::vec3 camPoint = camera->transform.getGlobalPosition();

			float angle = camera->transform.getEulerRotation().z;

			auto distanceAway = tanf(glm::radians(angle)) * camPoint.y;

			glm::vec3 flatForward = glm::normalize(glm::vec3(0.0f, camera->transform.forward().y, 0.0f));

			//glm::vec3 placePoint = 

			// Tan(angle) = o / a
			
			//toa

		}
	}
	else {

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
		ImGui::Combo("Brush Mode", (int*)&state, "None\0Brush\0\0");
		
		ImGui::End();
	}
}

LevelEditor::~LevelEditor()
{
}
