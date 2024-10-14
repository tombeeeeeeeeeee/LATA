#include "GamePlayCameraSystem.h"

#include "Camera.h"

#include "Utilities.h"

#include "Serialisation.h"
#include "EditorGUI.h"

GameplayCameraSystem::GameplayCameraSystem(toml::table table)
{
	cameraMoveSpeed = Serialisation::LoadAsFloat(table["cameraMoveSpeed"]);
	cameraZoomInSpeed = Serialisation::LoadAsFloat(table["cameraZoomInSpeed"]);
	cameraZoomOutSpeed = Serialisation::LoadAsFloat(table["cameraZoomOutSpeed"]);
	cameraZoomScale = Serialisation::LoadAsFloat(table["cameraZoomScale"]);
	cameraZoomMinimum = Serialisation::LoadAsFloat(table["cameraZoomMinimum"]);
	cameraZoomMaximum = Serialisation::LoadAsFloat(table["cameraZoomMaximum"]);
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
		zoomScale = glm::clamp(glm::length(deltaPos) * cameraZoomScale, cameraZoomMinimum, cameraZoomMaximum);
		target = syncTransform.getGlobalPosition() + (deltaPos * 0.5f);
		[[fallthrough]];

	case Camera::targetingPosition: 

		if(camera.orthoScale < zoomScale)
			camera.orthoScale = Utilities::Lerp(camera.orthoScale, zoomScale, cameraZoomOutSpeed);
		else
			camera.orthoScale = Utilities::Lerp(camera.orthoScale, zoomScale, cameraZoomInSpeed);

		glm::vec3 pos = Utilities::Lerp(cameraPositionDelta + target, camera.transform.getPosition(), cameraMoveSpeed);
		camera.transform.setPosition(pos);
		break;

	default:
		break;
	}
}

//void GameplayCameraSystem::ChangeCameraState(Camera& camera, Camera::State state)
//{
//}

void GameplayCameraSystem::GUI()
{
	ImGui::DragFloat("Minimum Camera Zoom", &cameraZoomMinimum);
	ImGui::DragFloat("Maximum Camera Zoom", &cameraZoomMaximum);
	ImGui::DragFloat("Zoom Intensity While targetting", &cameraZoomScale);
	ImGui::DragFloat("Zoom In Speed", &cameraZoomInSpeed);
	ImGui::DragFloat("Zoom Out Speed", &cameraZoomOutSpeed);
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
	{ "cameraZoomInSpeed", cameraZoomInSpeed },
	{ "cameraZoomOutSpeed", cameraZoomOutSpeed },
	{ "cameraZoomScale", cameraZoomScale },
	{ "cameraZoomMinimum" , cameraZoomMinimum },
	{ "cameraZoomMaximum" , cameraZoomMaximum },
	{ "cameraPositionDelta", Serialisation::SaveAsVec3(cameraPositionDelta) },
	{ "viewAngle", Serialisation::SaveAsVec3(viewAngle)},
	};
}
