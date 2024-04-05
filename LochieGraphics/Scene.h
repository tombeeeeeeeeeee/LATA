#pragma once

#include "SceneObject.h"
#include "Camera.h"
#include "Light.h"

#include "GUI.h"

#include "Maths.h"

#include <string>


class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	Camera* camera;
	GUI gui;
	std::vector<SceneObject*> sceneObjects;
	std::vector<Light*> lights;

	unsigned int* windowWidth;
	unsigned int* windowHeight;

	virtual void Start() {};
	virtual void Update(float delta) = 0;
	// TODO: Maybe move some of the GUI code that is in the GUI class here
	virtual void GUI() {};

	Scene();
	virtual ~Scene();

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	glm::mat4 viewProjection;
};
