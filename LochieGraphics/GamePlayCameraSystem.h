#pragma once

#include "Camera.h"

#include "Utilities.h"

class Transform;

class GameplayCameraSystem
{
public:
	glm::vec3 cameraPositionDelta;
	glm::vec3 viewAngle = { 0.0f, 0.0f, 0.0f };
	glm::vec3 target = { 0.0f, 0.0f, 0.0f};
	glm::vec3 anchor = { 0.0f, 0.0f, 0.0f };
	glm::vec2 constraint = { 0.0f, 0.0f };
	bool anchoring = false;
	float cameraZoomScale = 0.2f;
	float cameraZoomMinimum = 0.5f;
	float cameraZoomSpeed = 0.33f;
	float cameraMoveSpeed = 0.33f;

	GameplayCameraSystem() {};
	GameplayCameraSystem(toml::table table);

	void setCameraPositionDelta(glm::vec3 pos) { cameraPositionDelta = pos; };

	void setTarget(glm::vec3 _target) { target = _target; };

	void Update(
		Camera& camera, 
		Transform& eccoTransform, Transform& syncTransform,
		float zoomScale
	);

	void ChangeCameraState(Camera& camera, Camera::State state);

	void GUI();
	toml::table Serialise();
private:
	Camera* cam = nullptr;

};