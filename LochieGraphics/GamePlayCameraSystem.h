#pragma once

#include "Transform.h"
#include "Camera.h"
#include "Utilities.h"

class GameplayCameraSystem
{
public:
	glm::vec3 cameraPositionDelta;
	glm::quat cameraRotationWhileTargeting;
	glm::vec3 target = { 0.0f, 0.0f, 0.0f};
	float cameraZoomScale = 0.2f;
	float cameraZoomMinimum = 0.5f;
	float cameraZoomSpeed = 0.33f;
	float cameraMoveSpeed = 0.33f;


	void setCameraPositionDelta(glm::vec3 pos) { cameraPositionDelta = pos; };

	void setTarget(glm::vec3 _target) { target = _target; };

	void Update(
		Camera& camera, 
		Transform& eccoTransform, Transform& syncTransform,
		float zoomScale
	);

	void ChangeCameraState(Camera& camera, Camera::State state);

	void GUI();
};