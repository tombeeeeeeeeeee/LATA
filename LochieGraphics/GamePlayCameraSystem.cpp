#include "GamePlayCameraSystem.h"
#include "imgui.h"

void GameplayCameraSystem::Update(Camera& camera, Transform& eccoTransform, Transform& syncTransform, float zoomScale)
{
	switch (camera.state)
	{
	case Camera::targetingPlayers:

		glm::vec3 deltaPos = eccoTransform.getGlobalPosition() - syncTransform.getGlobalPosition();
		zoomScale = glm::max(glm::length(deltaPos) * cameraZoomScale, cameraZoomMinimum);
		target = syncTransform.getGlobalPosition() + (deltaPos * 0.5f);
		
	case Camera::targetingPosition: 

		camera.orthoScale = Utilities::Lerp(zoomScale, camera.orthoScale, cameraZoomSpeed);
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
	ImGui::DragFloat3("Ortho Camera Position", &cameraPositionDelta[0], 0.2f);
}
