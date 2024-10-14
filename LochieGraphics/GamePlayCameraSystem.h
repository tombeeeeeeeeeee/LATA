#pragma once

#include "Maths.h"

class Transform;
class Camera;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class GameplayCameraSystem
{
public:
	glm::vec3 cameraPositionDelta = {550.0f, 1000.0f, 750.0f};
	glm::vec3 viewAngle = { -135.0f, 35.0f, -120.0f };
	glm::vec3 target = { 0.0f, 0.0f, 0.0f};
	glm::vec3 anchor = { 0.0f, 0.0f, 0.0f };
	glm::vec2 constraint = { 0.0f, 0.0f };
	bool anchoring = false;
	float cameraZoomScale = 0.13f;
	float cameraZoomMinimum = 125.0f;
	float cameraZoomMaximum = 205.0f;
	float cameraZoomInSpeed = 0.33f;
	float cameraZoomOutSpeed = 0.01f;
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

	//void ChangeCameraState(Camera& camera, Camera::State state);

	void GUI();
	toml::table Serialise();
private:
	Camera* cam = nullptr;

};