#pragma once

#include "imguiStuff.h"

class Scene;
class Camera;

class GUI
{
public:
	Scene* scene;

	void Update();

private:

	void ResourceMenu();
	void TestMenu();
	void CameraMenu();

	void CameraGUI(Camera* camera);
};

