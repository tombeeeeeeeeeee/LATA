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

private:
	bool showResourceMenu = false;
	void ResourceMenu();

	bool showTestMenu = false;
	void TestMenu();
	
	bool showCameraMenu = false;
	void CameraMenu();
	
	int sceneObjectSelectedIndex = 0;
	bool showSceneObjectMenu = false;
	void SceneObjectMenu();
	
	int lightSelectedIndex = 0;
	bool showLightMenu = false;
	void LightMenu();
	
	bool showHierarchyMenu = false;
	void HierarchyMenu();
	void TransformTree(SceneObject* sceneObject);
	void TransformDragDrop(SceneObject* sceneObject);
	ImGuiTreeNodeFlags baseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
};

