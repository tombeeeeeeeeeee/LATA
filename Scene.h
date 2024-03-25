#pragma once

#include "SceneObject.h"
#include "Camera.h"

#include "GUI.h"

#include "Maths.h"

#include <string>


class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	Camera* camera;
	friend GUI;
	GUI gui;
	std::vector<SceneObject*> sceneObjects;

	virtual void Start() {};
	virtual void Update(float delta) = 0;

	Scene();
	virtual ~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	glm::mat4 viewProjection;
};
