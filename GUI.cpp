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
	SceneObjectGUI(scene->sceneObjects[sceneObjectSelectedIndex]);

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
	(scene->lights[lightSelectedIndex])->GUI();

	ImGui::End();
}

void GUI::SceneObjectGUI(SceneObject* sceneObject)
{
	sceneObject->transform.GUI();
	for (auto i = sceneObject->parts.begin(); i != sceneObject->parts.end(); i++)
	{
		(*i)->GUI();
	}
}