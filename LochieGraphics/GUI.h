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

	void ResourceMenu();
	void TestMenu();
	void CameraMenu();
	//TODO: functions will probably break if the vectors are empty
	int sceneObjectSelectedIndex = 0;
	void SceneObjectMenu();
	int lightSelectedIndex = 0;
	void LightMenu();
	void HierarchyMenu();
	void TransformTree(SceneObject* sceneObject);
	ImGuiTreeNodeFlags baseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
};

