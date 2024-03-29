#include "GUI.h"

#include "SceneObject.h"
#include "Scene.h"
#include "ModelRenderer.h"
#include "MultiModelRenderer.h"

#include "ResourceManager.h"

//TODO: move some of these to their own classes
void GUI::Update()
{
	TestMenu();
	ResourceMenu();
	CameraMenu();
	SceneObjectMenu();
	LightMenu();
}

void GUI::ResourceMenu()
{
	if (!ImGui::Begin("Resource Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	ResourceManager::GUI();

	ImGui::End();
}

void GUI::TestMenu()
{
	if (!ImGui::Begin("Controls Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();	
		return;
	}

	ImGui::Text("Look around with the mouse");
	ImGui::Text("Use keyboard (WASD) to move the camera around");
	ImGui::Text("Press the ALT key (left) to toggle between moving the mouse / camera");

	ImGui::End();
}

void GUI::CameraMenu()
{
	if (!ImGui::Begin("Camera Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	scene->camera->GUI();

	ImGui::End();
}

void GUI::SceneObjectMenu()
{
	if (!ImGui::Begin("Scene Object Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	if (ImGui::SliderInt("Scene Object Index", &sceneObjectSelectedIndex, 0, scene->sceneObjects.size()-1)) {
		sceneObjectSelectedIndex = glm::clamp(sceneObjectSelectedIndex, 0, (int)scene->sceneObjects.size()-1);
	}
	scene->sceneObjects[sceneObjectSelectedIndex]->GUI();

	ImGui::End();
}

void GUI::LightMenu()
{
	if (!ImGui::Begin("Light Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	if (ImGui::SliderInt("Light selected", &lightSelectedIndex, 0, scene->lights.size() - 1)) {
		lightSelectedIndex = glm::clamp(lightSelectedIndex, 0, (int)scene->lights.size() - 1);
	}
	scene->lights[lightSelectedIndex]->GUI();

	ImGui::End();
}