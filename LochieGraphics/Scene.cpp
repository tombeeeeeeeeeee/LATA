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
	for (int i = 0; i < sceneObjects.size(); i++)
	{
		delete sceneObjects[i];
	}
}

void Scene::Save()
{
	std::ofstream file("scene.txt");

	file << "Camera\n";

	file << camera->position.x << " " << camera->position.y << " " << camera->position.z << "\n";

	file << sceneObjects.size() << "\n";
	for (auto i = sceneObjects.begin(); i < sceneObjects.end(); i++)
	{
		file << "SceneObject\n";
		file << (*i)->name << "\n";
	}

	file.close();
}

void Scene::Load()
{
	// TODO: Remove everything in the scene first
	for (int i = 0; i < sceneObjects.size(); i++)
	{
		delete sceneObjects[i];
	}
	sceneObjects.clear();

	std::ifstream file("scene.txt");
	if (!file.is_open()) { 
		std::cout << "Error, save file to load not found\n";
	}
	
	std::string word;
	
	file >> word; // Camera
	file >> word;
	float x = stof(word);
	file >> word;
	float y = stof(word);
	file >> word;
	float z = stof(word);
	camera->position = { x, y, z };

	file >> word; // SceneObject count
	int sceneObjectCount = stoi(word);
	sceneObjects.reserve(sceneObjectCount);
	for (int i = 0; i < sceneObjectCount; i++)
	{
		SceneObject* newSceneObject = new SceneObject();
		sceneObjects.push_back(newSceneObject);
		file >> word; // SceneObject
		std::getline(file, word); // Empty string, end of the current line
		std::getline(file, word); // Name
		newSceneObject->name = word;



		//newSceneObject.
	}

	file.close();
}
