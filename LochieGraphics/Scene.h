#pragma once

#include "SceneObject.h"
#include "Camera.h"
#include "Light.h"
#include "Transform.h"

#include "Maths.h"
#include "GUI.h"

#include <string>


class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	Camera* camera;
	GUI gui;
	std::vector<SceneObject*> sceneObjects;
	std::vector<Light*> lights;
	std::vector<Shader*> shaders;

	unsigned int* windowWidth;
	unsigned int* windowHeight;

	virtual void Start() {};
	virtual void Update(float delta) = 0;
	virtual void Draw() = 0;
	// TODO: Maybe move some of the GUI code that is in the GUI class here
	void BaseGUI();
	virtual void GUI() {};

	Scene();
	virtual ~Scene();

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	void Save();
	void Load();
};
