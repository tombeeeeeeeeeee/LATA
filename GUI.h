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
	int sceneObjectSelectedIndex;
	void SceneObjectMenu();

	void CameraGUI(Camera* camera);
	void SceneObjectGUI(SceneObject* sceneObject);
	void TransformGUI(Transform* transform, std::string tag);
	void ModelRendererGUI(ModelRenderer* modelRenderer);
};

