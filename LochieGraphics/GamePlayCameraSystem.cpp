#include "GamePlayCameraSystem.h"
#include "Serialisation.h"
#include "SceneManager.h"
#include "EditorGUI.h"

GameplayCameraSystem::GameplayCameraSystem(toml::table table)
{
	cameraMoveSpeed = Serialisation::LoadAsFloat(table["cameraMoveSpeed"]);
	cameraZoomSpeed = Serialisation::LoadAsFloat(table["cameraZoomSpeed"]);
	cameraZoomScale = Serialisation::LoadAsFloat(table["cameraZoomScale"]);
	cameraZoomMinimum = Serialisation::LoadAsFloat(table["cameraZoomMinimum"]);
	cameraPositionDelta = Serialisation::LoadAsVec3(table["cameraPositionDelta"]);
	viewAngle = Serialisation::LoadAsVec3(table["viewAngle"]);
}

void GameplayCameraSystem::Update(Camera& camera, Transform& eccoTransform, Transform& syncTransform, float zoomScale)
{
	cam = &camera;
	switch (camera.state)
	{
	case Camera::targetingPlayers:
		
		camera.transform.setEulerRotation(viewAngle);
		glm::vec3 deltaPos = eccoTransform.getGlobalPosition() - syncTransform.getGlobalPosition();
		zoomScale = glm::max(glm::length(deltaPos) * cameraZoomScale, cameraZoomMinimum);
		target = syncTransform.getGlobalPosition() + (deltaPos * 0.5f);
		
	case Camera::targetingPosition: 

		camera.orthoScale = Utilities::Lerp(camera.orthoScale, zoomScale, cameraZoomSpeed);
		glm::vec3 pos = Utilities::Lerp(cameraPositionDelta + target, camera.transform.getPosition(), cameraMoveSpeed);
		camera.transform.setPosition(pos);
		break;

	default:
		break;
	}
}

void GameplayCameraSystem::ChangeCameraState(Camera& camera, Camera::State state)
{

}

void GameplayCameraSystem::GUI()
{
	ImGui::DragFloat("Minimum Camera Zoom", &cameraZoomMinimum);
	ImGui::DragFloat("Zoom Intensity While targetting", &cameraZoomScale);
	ImGui::DragFloat("Zoom Speed", &cameraZoomSpeed);
	ImGui::DragFloat("Camera Move Speed While targetting", &cameraMoveSpeed);

	if (cam)
	{
		ImGui::DragFloat3("Targeting Camera Position", &cameraPositionDelta[0], 0.2f);
		if (ImGui::Button("Bake Ortho Camera Position"))
		{
			cameraPositionDelta = cam->transform.getGlobalPosition();
		}
		ImGui::DragFloat3("Targeting Camera Angles", &viewAngle[0]);
		if (ImGui::Button("Bake Ortho Camera Angle"))
		{
			viewAngle = cam->transform.getEulerRotation();
		}
	}
}

toml::table GameplayCameraSystem::Serialise()
{
	return toml::table{
	{ "cameraMoveSpeed", cameraMoveSpeed },
	{ "cameraZoomSpeed", cameraZoomSpeed },
	{ "cameraZoomScale", cameraZoomScale },
	{ "cameraZoomMinimum" , cameraZoomMinimum },
	{ "cameraPositionDelta", Serialisation::SaveAsVec3(cameraPositionDelta) },
	{ "viewAngle", Serialisation::SaveAsVec3(viewAngle)},
	};
}
