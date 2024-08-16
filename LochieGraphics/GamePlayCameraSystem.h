#pragma once

#include "Transform.h"
#include "Camera.h"
#include "Utilities.h"

class GameplayCameraSystem
{
public:
	glm::vec3 initialCameraPos;
	glm::quat initialCameraRot;
	glm::vec3 target = { 0.0f, 0.0f, 0.0f};
	float cameraZoomScale = 1.0f;
	float cameraZoomMinimum = 0.5f;
	float cameraZoomSpeed = 0.33f;
	float cameraMoveSpeed = 0.33f;

	void Initialise(
		Camera& camera
	)
	{
		initialCameraPos = camera.transform.getGlobalPosition();
		initialCameraRot = camera.transform.getRotation();
	};

	void setInitialCameraSpot(glm::vec3 pos) {initialCameraPos = pos; };
	void setInitialCameraRot(glm::quat rot) { initialCameraRot = rot; };

	void setTarget(glm::vec3 _target) { target = _target; };

	void Update(
		Camera& camera, 
		Transform& eccoTransform, Transform& syncTransform,
		float zoomScale
	);
};