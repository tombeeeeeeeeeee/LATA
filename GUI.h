#pragma once

#include "imguiStuff.h"

class Scene;
class Camera;
class SceneObject;
class Transform;

class GUI
{
public:
	Scene* scene;

	void Update();

private:

	void ResourceMenu();
	void TestMenu();
	void CameraMenu();
	int sceneObjectSelectedIndex;
	void SceneObjectMenu();

	void CameraGUI(Camera* camera);
	void TransformGUI(Transform* transform);
	void SceneObjectGUI(SceneObject* sceneObject);
};

