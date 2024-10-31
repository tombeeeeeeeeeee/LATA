#include "GamePlayCameraSystem.h"

#include "Camera.h"

#include "Utilities.h"

#include <iostream>
#include "Serialisation.h"
#include "EditorGUI.h"
#include "ExtraEditorGUI.h"
#include "Paths.h"

#include <iostream>
#include <filesystem>
#include <ostream>
#include <fstream>


void GameplayCameraSystem::Load(toml::table table)
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

	if (isnan(syncTransform.getGlobalPosition().x) || isnan(eccoTransform.getGlobalPosition().x))
	{
		std::cout << "Is Nan On Camera";
	}

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
	std::vector<std::string> loadPaths = {};
	std::vector<std::string*> loadPathsPointers = {};
	loadPaths.clear();
	loadPathsPointers.clear();

	for (auto& i : std::filesystem::directory_iterator(Paths::systemPath))
	{
		loadPaths.push_back(i.path().generic_string().substr(Paths::systemPath.size()));
		if (loadPaths.back().substr(loadPaths.back().size() - Paths::cameraSystemExtension.size()) != Paths::cameraSystemExtension) {
			loadPaths.erase(--loadPaths.end());
			continue;
		}
		loadPaths.back() = loadPaths.back().substr(0, loadPaths.back().size() - Paths::cameraSystemExtension.size());
	}
	for (auto& i : loadPaths)
	{
		loadPathsPointers.push_back(&i);
	}

	std::string* selected = &filename;

	ExtraEditorGUI::InputSearchBox(loadPathsPointers.begin(), loadPathsPointers.end(), &selected, "Filename", Utilities::PointerToString(&loadPathsPointers));
	filename = *selected;
	if (ImGui::Button("Save##CameraSystems")) {

		std::ofstream file(Paths::systemPath + filename + Paths::cameraSystemExtension);

		toml::table table = Serialise();

		file << table << '\n';

		file.close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##CameraSystem")) {
		if (selected)
		{
			std::ifstream file(Paths::systemPath + filename + Paths::cameraSystemExtension);

			toml::table data = toml::parse(file);

			Load(data);

			file.close();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save as##CameraSystem"))
	{
		saveAs = true;
	}


	SaveAsGUI();

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

void GameplayCameraSystem::SaveAsGUI()
{
	if (saveAs)
	{
		ImGui::OpenPopup("Save Camera System As");
		saveAs = false;
	}
	if (ImGui::BeginPopupModal("Save Camera System As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::InputText("File ", &newFilename);

		if (ImGui::Button("Save"))
		{
			if (newFilename != "") {
				std::ofstream file(Paths::systemPath + newFilename + Paths::cameraSystemExtension);
				filename = newFilename;
				toml::table table = Serialise();

				file << table << '\n';

				file.close();
				ImGui::CloseCurrentPopup();

				newFilename = "";
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			newFilename = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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
