#include "GUI.h"

#include "SceneObject.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "Lights.h"
#include "Ecco.h"
#include "Sync.h"
// Needed for the physics menu
#include "Collider.h"
#include "UserPreferences.h"
#include "PrefabManager.h"
#include "ImGuiStyles.h"
#include "SceneManager.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Paths.h"

#include "ImGuizmo.h"

#include <iostream>
#include <filesystem>

using Utilities::PointerToString;

//TODO: move some of these to their own classes
void GUI::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	// TODO: GUI Shouldn't exist for a build version
	//if (true) { return; }
	if (glfwGetKey(SceneManager::window, GLFW_KEY_RIGHT_ALT))
	{
		if (!disableGUIHeld)
			scene->displayGUI = !scene->displayGUI;
		disableGUIHeld = true;
	}
	else disableGUIHeld = false;

	if (!scene->displayGUI) {
		return;
	}

	// TODO: Rebind-able key
	if (glfwGetKey(SceneManager::window, GLFW_KEY_DELETE) && !ImGui::GetIO().WantCaptureKeyboard) {
		if (sceneObjectSelected) {
			scene->DeleteSceneObject(sceneObjectSelected->GUID);
		}
	}

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
			ImGui::MenuItem("Audio Menu", NULL, &showAudioMenu);
			ImGui::MenuItem("Camera Menu", NULL, &showCameraMenu);
			ImGui::MenuItem("Enemy System", NULL, &showEnemyMenu);
			ImGui::MenuItem("Health System", NULL, &showHealthSystemMenu);
			if (ImGui::MenuItem("Hierarchy", NULL, &showHierarchy)) {
				showSceneObject = showHierarchy;
			}
			ImGui::MenuItem("Light Menu", NULL, &showLightMenu);
			ImGui::MenuItem("Particle Menu", NULL, &showParticleMenu);
			ImGui::MenuItem("Physics System", NULL, &showPhysicsMenu);
			ImGui::MenuItem("Prefabs Menu", NULL, &showPrefabMenu);
			ImGui::MenuItem("Render System", NULL, &showRenderSystemMenu);
			ImGui::MenuItem("Resource Menu", NULL, &showResourceMenu);
			if (ImGui::MenuItem("SceneObject Menu", NULL, &showSceneObject)) {
				showHierarchy = showSceneObject;
			}
			ImGui::MenuItem("Style Editor Menu", NULL, &showStyleMenu);
			ImGui::MenuItem("Test Menu", NULL, &showTestMenu);
			ImGui::MenuItem("User Prefs", NULL, &showUserPrefsMenu);

			ImGui::MenuItem("Imgui Demo", NULL, &showImguiExampleMenu);

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
		ImGui::MenuItem("Open in explorer");
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			std::string path = std::filesystem::current_path().string();
			system(("explorer " + path).c_str());

		}
		ImGui::EndMainMenuBar();
	}

	if (showTestMenu) { TestMenu(); }
	if (showResourceMenu) { ResourceMenu(); }
	if (showCameraMenu) { CameraMenu(); }
	if (showSceneObject) { SceneObjectMenu(); }
	else { focusSceneObjectMenu = false; }
	if (showLightMenu) { LightMenu(); }
	if (showHierarchy) { HierarchyMenu(); }
	else { moveSelection = 0; }
	if (showPhysicsMenu) { PhysicsMenu(); }
	if (showEnemyMenu) { EnemyMenu(); }
	if (showHealthSystemMenu) { HealthMenu(); }
	if (showImguiExampleMenu) { ImGui::ShowDemoWindow(); }
	if (showRenderSystemMenu) { scene->renderSystem.GUI(); }
	if (showPrefabMenu) {
		if (ImGui::Begin("Prefab Manager", &showPrefabMenu, defaultWindowFlags)) {
			PrefabManager::GUI();
		}
		ImGui::End();
	}
	if (showUserPrefsMenu) { 
		if (ImGui::Begin("User Preferences Menu", &showUserPrefsMenu, defaultWindowFlags)) {
			UserPreferences::GUI();
		}
		ImGui::End();
	}
	if (showParticleMenu) {
		if (ImGui::Begin("Particle Menu", &showParticleMenu, defaultWindowFlags)) {
			scene->particleSystem.GUI();
		}
		ImGui::End();
	}
	if (showAudioMenu) {
		if (ImGui::Begin("Audio Menu", &showAudioMenu, defaultWindowFlags)) {
			scene->audio.GUI();
		}
		ImGui::End();
	}
	if (showStyleMenu) {
		// Remove the auto resize as this window can get quite large
		if (ImGui::Begin("Style Editor Menu", &showStyleMenu, defaultWindowFlags & ~ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGuiStyles::GUI();
		}
		ImGui::End();
	}
	scene->GUI();

	// Update Potential Modals
	// TODO: Would be good if could be in the material class, perhaps the material input is shown via the material class, so that the material could handle the modal
	for (auto& i : ResourceManager::materials)
	{
		i.second.ModalGUI();
	}

	// TODO: Customisable keys
	if (glfwGetKey(SceneManager::window, GLFW_KEY_G) == GLFW_PRESS && !io.WantCaptureKeyboard) {
		if (!guizmoOperationChanged) {
			transformGizmoOperation = (transformGizmoOperation == ImGuizmo::OPERATION::TRANSLATE) ? ImGuizmo::OPERATION::ROTATE : ImGuizmo::OPERATION::TRANSLATE;
		}
		guizmoOperationChanged = true;
	}
	else {
		guizmoOperationChanged = false;
	}

	// TODO: Customisable keys
	if (glfwGetKey(SceneManager::window, GLFW_KEY_H) == GLFW_PRESS && !io.WantCaptureKeyboard) {
		if (!guizmoModeChanged) {
			transformGizmoMode = (transformGizmoMode == ImGuizmo::MODE::WORLD) ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;
		}
		guizmoModeChanged = true;
	}
	else {
		guizmoModeChanged = false;
	}

	// Transform GIZMO
	if (sceneObjectSelected) {
		int xOffset, yOffset;
		// TODO: There should be a window pos change callback, should justbe stored somewhere
		glfwGetWindowPos(SceneManager::window, &xOffset, &yOffset);
		ImGuizmo::SetOrthographic(scene->camera->InOrthoMode());
		ImGuizmo::SetRect((float)xOffset, (float)yOffset, io.DisplaySize.x, io.DisplaySize.y);
		sceneObjectSelected->transform()->Gizmo(SceneManager::view, SceneManager::projection, transformGizmoOperation, transformGizmoMode);
	}
}

SceneObject* GUI::getSelected() const
{
	return sceneObjectSelected;
}

const std::set<SceneObject*>& GUI::getMultiSelected() const
{
	return multiSelectedSceneObjects;
}

void GUI::setSelected(SceneObject* so)
{
	sceneObjectSelected = so;
	lastSelected = so;
	if (so) {
		modelHierarchySelected = nullptr;
		focusSceneObjectMenu = true;
	}

	multiSelectedSceneObjects.clear();
}

void GUI::setSelected(std::vector<SceneObject*> sceneObjects)
{
	if (sceneObjects.size() == 1) {
		setSelected(sceneObjects.front());
	}
	else {
		multiSelectedSceneObjects = std::set<SceneObject*>(sceneObjects.begin(), sceneObjects.end());
	}
}

void GUI::AddFromToSelection(SceneObject* from, SceneObject* to)
{
	// Have to 'wait' for the hierarchy to finish drawing, gets called properly at the end of hierarchy menu
	addRangeToSelection.push_back({ from, to });
}

void GUI::UpdateSelection()
{

	while (!addRangeToSelection.empty()) {
		std::vector<SceneObject*>::iterator one = std::find(hierarchySceneObjects.begin(), hierarchySceneObjects.end(), addRangeToSelection.front().first);
		std::vector<SceneObject*>::iterator two = std::find(hierarchySceneObjects.begin(), hierarchySceneObjects.end(), addRangeToSelection.front().second);
		unsigned long long oneIndex = std::distance(hierarchySceneObjects.begin(), one);
		unsigned long long twoIndex = std::distance(hierarchySceneObjects.begin(), two);
		if (oneIndex < hierarchySceneObjects.size() && twoIndex < hierarchySceneObjects.size()) {
			std::vector<SceneObject*>::iterator from = oneIndex < twoIndex ? one : two;
			std::vector<SceneObject*>::iterator to = (twoIndex < oneIndex ? one : two) + 1;
			for (auto& i = from; i != to; i++)
			{
				multiSelectedSceneObjects.insert(*i);
			}
		}
		addRangeToSelection.erase(addRangeToSelection.begin());
	}

	if (!multiSelectedSceneObjects.empty()) {
		if (sceneObjectSelected) {
			multiSelectedSceneObjects.insert(sceneObjectSelected);
			sceneObjectSelected = nullptr;
		}
	}
}

void GUI::MultiSceneObjectRightClickMenu()
{
	std::string multiRightClickPopupID = "multiRightClickPopup";
	if (openMultiSelectRightClickMenu) {
		ImGui::OpenPopup(multiRightClickPopupID.c_str());
		openMultiSelectRightClickMenu = false;
	}
	if (!ImGui::BeginPopup(multiRightClickPopupID.c_str())) {
		return;
	}
	bool removeSelection = false;
	SceneObject::MultiMenuGUI(multiSelectedSceneObjects, &removeSelection);
	if (removeSelection) {
		setSelected(nullptr);
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

	scene->gameCamSystem.GUI();



	ImGui::End();
}

void GUI::SceneObjectMenu()
{
	if (!ImGui::Begin("Scene Object Menu", &showSceneObject, defaultWindowFlags)) {
		if (focusSceneObjectMenu) {
			ImGui::SetWindowFocus();
			focusSceneObjectMenu = false;
		}

		ImGui::End();
		return;
	}
	focusSceneObjectMenu = false;

	if (multiSelectedSceneObjects.size() > 0) {
		ImGui::Text("Multi Object Editing not yet supported");
	}
	else if (sceneObjectSelected) {
		sceneObjectSelected->GUI();
	}
	else if (modelHierarchySelected) {
		if (lastSelected) {
			if (lastSelected->parts & Parts::modelRenderer && lastSelected->renderer()->model) {
				modelHierarchySelected->GUI(false, &lastSelected->renderer()->model->boneInfoMap);
			}
			else {
				modelHierarchySelected->GUI(false);
			}
		}
		else {
			std::cout << "Hierarchy selected but no object, infomation might be missing\n";
			modelHierarchySelected->GUI(false);
		}
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

	scene->directionalLight.GUI();

	ImGui::End();
}

void GUI::HierarchyMenu()
{
	if (!ImGui::Begin("Hierarchy Menu", &showHierarchy, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	hierarchySceneObjects.clear();

	ImGui::InputText("##Search", &hierarchyMenuSearch);
	if (ImGui::BeginItemTooltip()) {
		ImGui::Text("Search");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	std::string partsFilterPopupID = "partsFilterPopUp";
	if (partsFilter) {
		// TODO: Variable for the colour
		ImGui::PushStyleColor(0, { 0.7f, 0.3f, 0.1f, 1.0f });
	}
	std::string partsFilterLetter = partsFilter ? "F" : "A";
	bool partsFilterOpen = ImGui::Button((partsFilterLetter + "##Parts filter").c_str());
	if (partsFilter) {
		ImGui::PopStyleColor();
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::Text("Parts Filter");
		ImGui::EndTooltip();
	}
	if (partsFilterOpen) {
		ImGui::OpenPopup(partsFilterPopupID.c_str());
	}
	if (ImGui::BeginPopup(partsFilterPopupID.c_str())) {

		ImGui::CheckboxFlags("Animator##Parts Filter", &partsFilter, Parts::animator);
		ImGui::CheckboxFlags("Bollard##Parts Filter", &partsFilter, Parts::bollard);
		ImGui::CheckboxFlags("PointLight##Parts Filter", &partsFilter, Parts::pointLight);
		ImGui::CheckboxFlags("Collider##Parts Filter", &partsFilter, Parts::collider);
		ImGui::CheckboxFlags("Door##Parts Filter", &partsFilter, Parts::door);
		ImGui::CheckboxFlags("Ecco##Parts Filter", &partsFilter, Parts::ecco);
		ImGui::CheckboxFlags("Enemy##Parts Filter", &partsFilter, Parts::enemy);
		ImGui::CheckboxFlags("Exit Elevator##Parts Filter", &partsFilter, Parts::exitElevator);
		ImGui::CheckboxFlags("Plate##Parts Filter", &partsFilter, Parts::plate);
		ImGui::CheckboxFlags("PointLight##Parts Filter", &partsFilter, Parts::pointLight);
		ImGui::CheckboxFlags("RigidBody##Parts Filter", &partsFilter, Parts::rigidBody);
		ImGui::CheckboxFlags("Spawn Manager##Parts Filter", &partsFilter, Parts::spawnManager);
		ImGui::CheckboxFlags("Spotlight##Parts Filter", &partsFilter, Parts::spotlight);
		ImGui::CheckboxFlags("Sync##Parts Filter", &partsFilter, Parts::sync);

		ImGui::EndPopup();
	}

	ImGui::Unindent();
	ImGui::TreeNodeEx(("Root##" + PointerToString(this)).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
	if (ImGui::IsItemClicked()) {
		setSelected(nullptr);
	}
	ImGui::Indent();

	ImGui::BeginChild("Hierarchy List");

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
	for (auto& i : scene->sceneObjects)
	{
		if (!i.second) 
		{
			// TODO: Maybe error here
			// TODO: Maybe shouldn't be here, there is something similar near the end of load all sceneObjects in scene
			scene->DeleteSceneObject(i.first); continue;
		}
		if (i.second->transform()->getParent()) { continue; }

		TransformTree(i.second);
	}

	ImGui::Unindent();
	ImGui::TreeNodeEx(("+ NEW SCENEOBJECT##" + PointerToString(this)).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
	if (ImGui::IsItemClicked()) {
		setSelected(new SceneObject(scene));
	}
	ImGui::Indent();

	UpdateSelection();

	MultiSceneObjectRightClickMenu();

	if (moveSelection && sceneObjectSelected && ImGui::IsWindowFocused()) {
		auto selected = std::find(hierarchySceneObjects.begin(), hierarchySceneObjects.end(), sceneObjectSelected);
		unsigned long long selectedIndex = std::distance(hierarchySceneObjects.begin(), selected);
		unsigned long long desiredIndex = selectedIndex + moveSelection;
		if (desiredIndex < hierarchySceneObjects.size()) {
			setSelected(hierarchySceneObjects.at(desiredIndex));
		}
	}
	moveSelection = 0;

	ImGui::EndChild();

	ImGui::End();
}

void GUI::TransformTree(SceneObject* sceneObject)
{
	if (hierarchyMenuSearch != "") {
		if (Utilities::ToLower(sceneObject->name).find(Utilities::ToLower(hierarchyMenuSearch)) == std::string::npos) {
			return;
		}
	}
	if ((partsFilter & sceneObject->parts) != partsFilter) {
		return;
	}
	std::string tag = std::to_string(sceneObject->GUID);
	ImGuiTreeNodeFlags nodeFlags = baseNodeFlags;
	if (sceneObjectSelected == sceneObject || std::find(multiSelectedSceneObjects.begin(), multiSelectedSceneObjects.end(), sceneObject) != multiSelectedSceneObjects.end()) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	bool hasChildren = sceneObject->transform()->HasChildren() || (sceneObject->parts & Parts::modelRenderer && sceneObject->renderer()->model && UserPreferences::showModelHierarchy);
	if (!hasChildren) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	
	// TODO: Allow these colours to be changed
	ImVec4 textColour = ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Text];
	if (sceneObject->prefabStatus == SceneObject::PrefabStatus::origin) {
		textColour = { 0.0823529412f, 0.8235294118f, 0.4078431373f, 1.0f };
	}
	else if (sceneObject->prefabStatus == SceneObject::PrefabStatus::instance) {
		textColour = { 0.4980392157f, 0.8392156863f, 0.9921568627f, 1.0f };
	}
	else if (sceneObject->prefabStatus == SceneObject::PrefabStatus::missing) {
		textColour = { 0.7607843137f, 0.4039215686f, 0.4039215686f, 1.0f };
	}

	ImGui::PushStyleColor(0, textColour);
	bool nodeOpen = ImGui::TreeNodeEx((sceneObject->name + "##" + tag).c_str(), nodeFlags);
	hierarchySceneObjects.push_back(sceneObject);
	ImGui::PopStyleColor();
	
	
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
		bool shift = glfwGetKey(SceneManager::window, GLFW_KEY_LEFT_SHIFT);
		bool ctrl = glfwGetKey(SceneManager::window, GLFW_KEY_LEFT_CONTROL);

		if (shift && lastSelected) {
			AddFromToSelection(lastSelected, sceneObject);
			lastSelected = sceneObject;
		}
		else if (ctrl && (!multiSelectedSceneObjects.empty() || sceneObjectSelected)) {
			if (multiSelectedSceneObjects.find(sceneObject) != multiSelectedSceneObjects.end()) {
				multiSelectedSceneObjects.erase(sceneObject);
			}
			else {
				multiSelectedSceneObjects.insert(sceneObject);
				lastSelected = sceneObject;
			}
		}
		else {
			setSelected(sceneObject);
		}
	}

	std::string sceneObjectPopUpID = "SceneObjectRightClickPopUp##" + tag;

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
		if (multiSelectedSceneObjects.find(sceneObject) == multiSelectedSceneObjects.end()) {
			setSelected(sceneObject);
			ImGui::OpenPopup(sceneObjectPopUpID.c_str());
		}
		else {
			openMultiSelectRightClickMenu = true;
		}
	}

	if (ImGui::BeginPopup(sceneObjectPopUpID.c_str())) {
		sceneObject->MenuGUI();
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
	if (sceneObject->parts & Parts::modelRenderer && UserPreferences::showModelHierarchy) {
		auto model = sceneObject->renderer()->model;
		if (model) {
			TransformTree(sceneObject, &model->root);
		}
	}
	ImGui::TreePop();
}

void GUI::TransformTree(SceneObject* so, ModelHierarchyInfo* info)
{
	if (hierarchyMenuSearch != "") {
		if (Utilities::ToLower(info->name).find(Utilities::ToLower(hierarchyMenuSearch)) == std::string::npos) {
			return;
		}
	}
	std::string tag = Utilities::PointerToString(info);
	ImGuiTreeNodeFlags nodeFlags = baseNodeFlags;
	if (modelHierarchySelected == info /*|| std::find(multiSelectedSceneObjects.begin(), multiSelectedSceneObjects.end(), sceneObject) != multiSelectedSceneObjects.end()*/) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	bool hasChildren = !info->children.empty();
	if (!hasChildren) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool nodeOpen = ImGui::TreeNodeEx((info->name + "##" + tag).c_str(), nodeFlags);
	//hierarchySceneObjects.push_back(sceneObject);


	if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
	//	bool shift = glfwGetKey(SceneManager::window, GLFW_KEY_LEFT_SHIFT);
	//	bool ctrl = glfwGetKey(SceneManager::window, GLFW_KEY_LEFT_CONTROL);
	//
	//	if (shift && lastSelected) {
	//		AddFromToSelection(lastSelected, sceneObject);
	//		lastSelected = sceneObject;
	//	}
	//	else if (ctrl && (!multiSelectedSceneObjects.empty() || sceneObjectSelected)) {
	//		if (multiSelectedSceneObjects.find(sceneObject) != multiSelectedSceneObjects.end()) {
	//			multiSelectedSceneObjects.erase(sceneObject);
	//		}
	//		else {
	//			multiSelectedSceneObjects.insert(sceneObject);
	//			lastSelected = sceneObject;
	//		}
	//	}
	//	else {
		modelHierarchySelected = info;
		setSelected(nullptr);
		lastSelected = so;
	//	}
	}

	//std::string sceneObjectPopUpID = "SceneObjectRightClickPopUp##" + tag;

	//if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
	//	if (multiSelectedSceneObjects.find(sceneObject) == multiSelectedSceneObjects.end()) {
	//		setSelected(sceneObject);
	//		ImGui::OpenPopup(sceneObjectPopUpID.c_str());
	//	}
	//	else {
	//		openMultiSelectRightClickMenu = true;
	//	}
	//}

	//if (ImGui::BeginPopup(sceneObjectPopUpID.c_str())) {
	//	sceneObject->MenuGUI();
	//}

	//TransformDragDrop(sceneObject);

	if (!hasChildren || !nodeOpen) {
		return;
	}
	auto& children = info->children;
	for (auto child : children)
	{
		TransformTree(so, child);
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

void GUI::PhysicsMenu()
{
	if (!ImGui::Begin("Physics Menu", &showPhysicsMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}

	ImGui::DragFloat("Mininmum Collision Distance", &scene->physicsSystem.minCollisonDistance);
	//TODO SAVE AND LOAD
	ImGui::Checkbox("Display All Colliders", &scene->physicsSystem.displayAllColliders);

	int flagCount = (int)log2((int)CollisionLayers::count);

	ImGui::Text("0 : base");
	ImGui::Text("1 : enemy");
	ImGui::Text("2 : reflectiveSurface");
	ImGui::Text("3 : sync");
	ImGui::Text("4 : ecco");
	ImGui::Text("5 : trigger");
	ImGui::Text("6 : enemyProjectile");
	ImGui::Text("7 : softCover");
	ImGui::Text("8 : halfCover");

	ImGui::Text("");
	ImGui::Text(" ");
	for (int i = 0; i < flagCount; i++)
	{
		ImGui::SameLine();
		ImGui::Text((std::to_string(i) + "  ").c_str());
	}
	for (int i = 0; i < flagCount; i++)
	{
		ImGui::Text(std::to_string(i).c_str());
		for (int j = 0; j < flagCount; j++)
		{
			bool layerToggle = scene->physicsSystem.GetCollisionLayerIndexed(i, j);
			ImGui::SameLine();
			if (ImGui::Checkbox(("##" + std::to_string(i) + " " + std::to_string(j) + " collisionLayer").c_str(), &layerToggle))
			{
				scene->physicsSystem.SetCollisionLayerMaskIndexed(i, j, layerToggle);
			}
		}
	}

	ImGui::Text("");
	ImGui::TextColored({ 0.2f,0.8f,0.2f, 1.0f }, "Ask Tom for more collision layers");

	ImGui::DragInt("Collision Itterations", &scene->physicsSystem.collisionItterations, 0.5f, 0);
	ImGui::End();
}

void GUI::EnemyMenu()
{
	if (!ImGui::Begin("Enemy Menu", &showEnemyMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}
	EnemySystem& es = scene->enemySystem;

	es.GUI();
}

void GUI::HealthMenu()
{
	if(!ImGui::Begin("Health Menu", &showHealthSystemMenu, defaultWindowFlags)) {
		ImGui::End();
		return;
	}
	scene->healthSystem.GUI();
}
