#include "GUI.h"

#include "Scene.h"

#include "ResourceManager.h"

void GUI::Update()
{
	TestMenu();
	ResourceMenu();
	CameraMenu();
}

void GUI::ResourceMenu()
{
	if (!ImGui::Begin("Resource Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	if (ImGui::BeginTable("Resource Textures", 3)) {
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("ID");

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Path");

		ImGui::TableSetColumnIndex(2);
		ImGui::Text("Type");

		for (auto i = ResourceManager::textures.begin(); i != ResourceManager::textures.end(); i++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text(std::to_string(i->second.ID).c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text(i->first.c_str());
			
			ImGui::TableSetColumnIndex(2);
			ImGui::Text(Texture::TypeNames.find(i->second.type)->second.c_str());
		}
		ImGui::EndTable();
	}
	ImGui::NewLine();

	for (auto i = ResourceManager::materials.begin(); i != ResourceManager::materials.end(); i++)
	{
		for (auto j = i->second.textures.begin(); j != i->second.textures.end(); j++)
		{
			ImGui::Text(std::to_string((*j)->ID).c_str());
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	ImGui::NewLine();

	for (auto i = ResourceManager::shaders.begin(); i != ResourceManager::shaders.end(); i++)
	{
		ImGui::Text(i->first.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(i->second.ID).c_str());
	}

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

	ImGui::End();
}

void GUI::CameraMenu()
{
	if (!ImGui::Begin("Camera Menu", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	CameraGUI(scene->camera);

	ImGui::End();
}

void GUI::CameraGUI(Camera* camera)
{
	ImGui::DragFloat3("Position##Camera", &camera->position[0]);
	
	ImGui::BeginDisabled();
	ImGui::DragFloat3("Front##Camera", &camera->front[0]);
	ImGui::DragFloat3("Up##Camera", &camera->up[0]);
	ImGui::DragFloat3("Right##Camera", &camera->right[0]);
	ImGui::EndDisabled();

	
	if (ImGui::DragFloat3("World up##Camera", &camera->worldUp[0])) {
		camera->UpdateVectors();
	}

	if (ImGui::DragFloat("Yaw##Camera", &camera->yaw)) {
		camera->UpdateVectors();
	}

	if (ImGui::DragFloat("Pitch##Camera", &camera->pitch)) {
		camera->UpdateVectors();
	}

	ImGui::DragFloat("Movement Speed##Camera", &camera->movementSpeed);
	ImGui::DragFloat("Sensitivity##Camera", &camera->sensitivity);
	ImGui::DragFloat("FOV##Camera", &camera->fov);
}

