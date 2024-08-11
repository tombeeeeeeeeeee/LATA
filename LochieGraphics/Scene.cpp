#include "Scene.h"

#include "imgui.h"

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
	//TODO: clear sceneobjects.
	//for (int i = 0; i < sceneObjects.size(); i++)
	//{
	//	delete sceneObjects[i];
	//}
}

void Scene::Save()
{
	std::ofstream file("TestScene.toml");

	auto savedShaders = toml::array();
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		savedShaders.push_back((*i)->Serialise());
	}

	file << toml::table{ {"Shaders", savedShaders} } << "\n\n";
	
	file << toml::table{ { "Camera", camera->Serialise() } } << "\n\n";

	auto savedSceneObjects = toml::array();

	//TODO: add partBitMask to sceneObjects.
	//TODO: Make serialise function in SceneManager? Or is it Scene?
	//for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	//{
	//	savedSceneObjects.push_back((*i)->Serialise());
	//}
	
	file << toml::table{ { "SceneObjects", savedSceneObjects } };

	file.close();
}

void Scene::Load()
{
	std::ifstream file("TestScene.toml");

	toml::table data = toml::parse(file);

	auto cam = data["Camera"];
	auto camPos = cam["position"].as_array();
	//camera->position = { camPos->at(0).value_or<float>(0.0f), camPos->at(1).value_or<float>(0.f), camPos->at(2).value<float>().value()};
	auto camRot = cam["rotation"].as_array();
	//camera->yaw = camRot->at(0).value_or<float>(0.f);
	//camera->pitch = camRot->at(1).value_or<float>(0.f);
	
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
