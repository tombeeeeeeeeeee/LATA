#pragma once

#include "Camera.h"

#include "Maths.h"

#include <string>

class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	Camera* camera;

	virtual void Start() {};
	virtual void Update(float delta) = 0;

	Scene() = default;
	virtual ~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	glm::mat4 viewProjection;
};
