#pragma once

#include "Maths.h"
#include <string>

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
	glm::vec3 cameraPositionDelta = { 550.0f, 1000.0f, 750.0f };
	glm::vec3 viewAngle = { 45.5f, 215.0f, 0.0f };
	glm::vec3 target = { 0.0f, 0.0f, 0.0f};
	float cameraZoomScale = 0.13f;
	float cameraZoomMinimum = 125.0f;
	float cameraZoomMaximum = 205.0f;
	float cameraZoomInSpeed = 0.33f;
	float cameraZoomOutSpeed = 0.01f;
	float cameraMoveSpeed = 0.33f;

	// TODO: Save new values
	// These are values specfic to perspective, some above values are shared or just orthographic
	float cameraFov = 0.0f;
	float cameraZoomClosestDistance = 0.0f;
	float cameraZoomFurthestDistance = 0.0f;

	GameplayCameraSystem() {};
	void Load(toml::table table);

	void setCameraPositionDelta(glm::vec3 pos) { cameraPositionDelta = pos; };

	void setTarget(glm::vec3 _target) { target = _target; };

	void Update(
		Camera& camera, 
		Transform& eccoTransform, Transform& syncTransform,
		float zoomScale
	);

	//void ChangeCameraState(Camera& camera, Camera::State state);

	static bool FileSelector(std::string* filename);
	void GUI();
	void SaveAsGUI();
	toml::table Serialise();
	std::string filename = "";

	// Only needs to be used while the camera system is active
	glm::vec3 currentTarget = { 0.0f, 0.0f, 0.0f };
private:
	Camera* cam = nullptr;
	std::string newFilename = "";
	bool saveAs = false;

	// debug values to see in the debugger
	float desiredDistance = 0.0f;
	float spread = 0.0f;
	float distance = 0.0f;
	float currentDistance = 0.0f;
	glm::vec3 lerpedPos = { 0.0f, 0.0f, 0.0f };
};