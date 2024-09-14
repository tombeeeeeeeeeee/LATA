#include "GUI.h"

#include "SceneObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Utilities.h"

#include "ResourceManager.h"

#include <iostream>

using Utilities::PointerToString;

//TODO: move some of these to their own classes
void GUI::Update()
{
	// TODO: GUI Shouldn't exist for a build version
	//if (true) { return; }

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) {
				scene->Save();
			}
			if (ImGui::MenuItem("Load")) {
				scene->Load();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			ImGui::MenuItem("Test Menu", NULL, &showTestMenu);
			ImGui::MenuItem("Resource Menu", NULL, &showResourceMenu);
			ImGui::MenuItem("Camera Menu", NULL, &showCameraMenu);
			if (ImGui::MenuItem("SceneObject Menu", NULL, &showSceneObject)) {
				showHierarchy = showSceneObject;
			}
			ImGui::MenuItem("Light Menu", NULL, &showLightMenu);
			if (ImGui::MenuItem("Hierarchy Menu", NULL, &showHierarchy)) {
				showSceneObject = showHierarchy;
			}
			ImGui::MenuItem("Imgui Demo Window", NULL, &showImguiExampleMenu);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("SceneObject")) {
			if (ImGui::MenuItem("Create New")) {
				// NOTE: This is okay, it's the scene's responsibility to delete the sceneobjects
				// TODO: This looks bad, make a scene function instead so it can look bad somewhere else
				new SceneObject(scene);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (showTestMenu)         { TestMenu();              }
	if (showResourceMenu)     { ResourceMenu();          }
	if (showCameraMenu)       { CameraMenu();            }
	if (showSceneObject)      { SceneObjectMenu();       }
	if (showLightMenu)        { LightMenu();             }
	if (showHierarchy)        { HierarchyMenu();         }
	if (showImguiExampleMenu) { ImGui::ShowDemoWindow(); }
	scene->BaseGUI();
	scene->GUI();

	// Update Potential Modals
	// TODO: Would be good if could be in the material class, perhaps the material input is shown via the material class, so that the material could handle the modal
	for (auto& i : ResourceManager::materials)
	{
		i.second.ModalGUI();
	}
}

void GUI::ResourceMenu()
{
	if (!ImGui::Begin("Resource Menu", &showResourceMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	ResourceManager::GUI();

	// TODO: Make this graph better, have lines and stuff for specific frame rates
	// Graph range top is 20fps
	ImGui::PlotLines("Time per frame", SceneManager::frameTimes.elements, IM_ARRAYSIZE(SceneManager::frameTimes.elements), (int)SceneManager::frameTimes.position, nullptr, 0.00f, 0.05f, ImVec2(400.f, 80.0f));
	auto averageFrameTime = SceneManager::frameTimes.getMean();
	ImGui::Text(("Average Frame Time: " + std::to_string(averageFrameTime)).c_str());
	ImGui::Text(("Average FPS: " + std::to_string(1 / averageFrameTime)).c_str());

	ImGui::End();
}

void GUI::TestMenu()
{
	if (!ImGui::Begin("Controls Menu", &showTestMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	ImGui::Text("Look around with the mouse");
	ImGui::Text("Use keyboard (WASD) to move the camera around");
	ImGui::Text("Press the ALT key (left) to toggle between moving the mouse / camera");
	ImGui::Text("Hold right click to also switch moving the mouse/camera");

	ImGui::End();
}

void GUI::CameraMenu()
{
	if (!ImGui::Begin("Camera Menu", &showCameraMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	scene->camera->GUI();

	ImGui::End();
}

void GUI::SceneObjectMenu()
{
	if (!ImGui::Begin("Scene Object Menu", &showSceneObject, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	if (sceneObjectSelected) {
		sceneObjectSelected->GUI();
	}
	else {
		ImGui::Text("Select a Scene Object in the Hierarchy Menu");
	}

	ImGui::End();
}

void GUI::LightMenu()
{
	if (!ImGui::Begin("Light Menu", &showLightMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	if (ImGui::SliderInt("Light selected", &lightSelectedIndex, 0, scene->lights.size() ? ((int)(scene->lights.size() - 1)) : 0)) {
		lightSelectedIndex = glm::clamp(lightSelectedIndex, 0, scene->lights.size() ? ((int)(scene->lights.size() - 1)) : 0);
	}
	if (lightSelectedIndex < scene->lights.size() /*&& scene->lights[lightSelectedIndex] != nullptr*/) {
		scene->lights[lightSelectedIndex]->GUI();
	}

	ImGui::End();
}

void GUI::HierarchyMenu()
{
	if (!ImGui::Begin("Hierarchy Menu", &showHierarchy, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	ImGui::Unindent();
	ImGui::TreeNodeEx(("Root##" + PointerToString(this)).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
	if (ImGui::IsItemClicked()) {
		sceneObjectSelected = nullptr;
	}
	ImGui::Indent();

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Transform"))
		{
			//IM_ASSERT(payload->DataSize == sizeof(Transform*));
			Transform* droppedTransform = *(Transform**)payload->Data;
			std::cout << "Stopped dragging transform:  " << droppedTransform << "\n";
			droppedTransform->setParent(nullptr);
		}
		ImGui::EndDragDropTarget();
	}
	for (auto i = scene->sceneObjects.begin(); i != scene->sceneObjects.end(); i++)
	{
		if ((*i).second->transform()->getParent()) { continue; }

		TransformTree((*i).second);
	}

	ImGui::Unindent();
	ImGui::TreeNodeEx(("+ NEW SCENEOBJECT##" + PointerToString(this)).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
	if (ImGui::IsItemClicked()) {
		sceneObjectSelected = new SceneObject(scene);
	}
	ImGui::Indent();

	ImGui::End();
}

void GUI::TransformTree(SceneObject* sceneObject)
{
	std::string tag = std::to_string(sceneObject->GUID);
	ImGuiTreeNodeFlags nodeFlags = baseNodeFlags;
	if (sceneObjectSelected == sceneObject) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	bool hasChildren = sceneObject->transform()->HasChildren();
	if (!hasChildren) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	bool nodeOpen = ImGui::TreeNodeEx((sceneObject->name + "##" + tag).c_str(), nodeFlags);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
		sceneObjectSelected = sceneObject;
		std::cout << "Changed gui select!\n";
	}
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(("SceneObjectRightClickPopUp##" + tag).c_str());
	}
	if (ImGui::BeginPopup(("SceneObjectRightClickPopUp##" + tag).c_str())) {
		if (ImGui::MenuItem(("Delete##RightClick" + tag).c_str())) {
			scene->DeleteSceneObject(sceneObject->GUID);
		}
		ImGui::EndPopup();
	}


	TransformDragDrop(sceneObject);

	if (!hasChildren || !nodeOpen) {
		return;
	}
	auto children = sceneObject->transform()->getChildren();
	for (auto child : children)
	{
		TransformTree(child->getSceneObject());
	}
	ImGui::TreePop();
}

void GUI::TransformDragDrop(SceneObject* sceneObject)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		Transform* transform = sceneObject->transform();
		ImGui::SetDragDropPayload("Transform", &transform, sizeof(transform));
		ImGui::Text(("Transform of: " + sceneObject->name).c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Transform"))
		{
			Transform* droppedTransform = *(Transform**)payload->Data;
			droppedTransform->setParent(sceneObject->transform());
		}
		ImGui::EndDragDropTarget();
	}
}
