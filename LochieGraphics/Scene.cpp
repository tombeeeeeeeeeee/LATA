#include "Scene.h"

#include "imgui.h"

#include "toml.hpp"

#include <fstream>
#include <iostream>

void Scene::BaseGUI()
{

}

Scene::Scene()
{
	gui.scene = this;
}

Scene::~Scene()
{
	for (int i = 0; i < sceneObjects.size(); i++)
	{
		delete sceneObjects[i];
	}
}

void Scene::Save()
{
	std::ofstream file("TestScene.toml");

	auto savedShaders = toml::array();
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		savedShaders.push_back(
			toml::table{
				// TODO: Should not be converting a GUID (of type (unsigned long long) to type of (long long))
				{ "guid", (long long)(*i)->GUID },
				{ "vertex", (*i)->vertexPath },
				{ "fragment", (*i)->fragmentPath },
				{ "flags", (*i)->getFlag()}
			}
		);
	}

	file << toml::table{ {"Shaders", savedShaders} } << "\n\n";

	// TODO: Move to camera
	auto cam = toml::table{
		{ "Camera", toml::table {
			{ "position", toml::array{ camera->position.x, camera->position.y, camera->position.z } },
			{ "rotation", toml::array{ camera->yaw, camera->pitch } },
			{ "fov", camera->fov }
			}
		}
	};
	
	file << cam << "\n\n";

	auto savedSceneObjects = toml::array();
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		auto sceneObject = toml::table{
			{ "name", (*i)->name },
			{ "hasRenderer", (*i)->getRenderer() != nullptr}
		};
		savedSceneObjects.push_back(sceneObject);
	}
	
	file << toml::table{ { "SceneObjects", savedSceneObjects } };
	//file << "Camera\n";

	//file << camera->position.x << " " << camera->position.y << " " << camera->position.z << "\n";

	//file << sceneObjects.size() << "\n";
	//for (auto i = sceneObjects.begin(); i < sceneObjects.end(); i++)
	//{
	//	file << "SceneObject\n";
	//	file << (*i)->name << "\n";
	//}

	file.close();

}

void Scene::Load()
{
	std::ifstream file("TestScene.toml");

	toml::table data = toml::parse(file);

	auto cam = data["Camera"];
	auto camPos = cam["position"].as_array();
	camera->position = { camPos->at(0).value_or<float>(0.0f), camPos->at(1).value_or<float>(0.f), camPos->at(2).value<float>().value()};
	auto camRot = cam["rotation"].as_array();
	camera->yaw = camRot->at(0).value_or<float>(0.f);
	camera->pitch = camRot->at(1).value_or<float>(0.f);
	
	camera->UpdateVectors();
	


	//// TODO: Remove everything in the scene first
	//for (int i = 0; i < sceneObjects.size(); i++)
	//{
	//	delete sceneObjects[i];
	//}
	//sceneObjects.clear();

	//if (!file.is_open()) {
	//	std::cout << "Error, save file to load not found\n";
	//}

	//std::string word;

	//file >> word; // Camera
	//file >> word;
	//float x = stof(word);
	//file >> word;
	//float y = stof(word);
	//file >> word;
	//float z = stof(word);
	//camera->position = { x, y, z };

	//file >> word; // SceneObject count
	//int sceneObjectCount = stoi(word);
	//sceneObjects.reserve(sceneObjectCount);
	//for (int i = 0; i < sceneObjectCount; i++)
	//{
	//	SceneObject* newSceneObject = new SceneObject();
	//	sceneObjects.push_back(newSceneObject);
	//	file >> word; // SceneObject
	//	std::getline(file, word); // Empty string, end of the current line
	//	std::getline(file, word); // Name
	//	newSceneObject->name = word;



	//	//newSceneObject.
	//}

	file.close();

}
