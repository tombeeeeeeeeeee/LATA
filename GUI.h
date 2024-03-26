#pragma once

#include "imguiStuff.h"

#include <string>

class Scene;
class Camera;
class SceneObject;
class Transform;
class ModelRenderer;

class GUI
{
public:
	Scene* scene;

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

	void SceneObjectGUI(SceneObject* sceneObject);
};

