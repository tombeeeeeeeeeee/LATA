#pragma once

#include "imgui.h"

#include <string>

class Scene;
class SceneObject;

// TODO: Not all the menus need to have the word menu in their name
class GUI
{
public:
	Scene* scene;

	SceneObject* sceneObjectSelected = nullptr;

	void Update();

	bool showResourceMenu = false;
	bool showTestMenu = false;
	bool showCameraMenu = false;
	bool showHierarchy = false;
	bool showSceneObject = false;
	bool showLightMenu = false;
private:

	ImGuiWindowFlags defaultWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;

	void ResourceMenu();

	void TestMenu();
	
	void CameraMenu();
	
	//bool showSceneObjectMenu = false;
	void SceneObjectMenu();
	
	int lightSelectedIndex = 0;
	void LightMenu();
	
	//bool showHierarchyMenu = false;
	void HierarchyMenu();
	void TransformTree(SceneObject* sceneObject);
	void TransformDragDrop(SceneObject* sceneObject);
	ImGuiTreeNodeFlags baseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
};

