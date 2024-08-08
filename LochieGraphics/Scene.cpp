#include "Scene.h"

#include "ResourceManager.h"

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
	// TODO: Name is user selected, perhaps have a file opening dialog, see https://github.com/mlabbe/nativefiledialog 
	std::ofstream file("TestScene.toml");

	auto savedShaders = toml::array();
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		savedShaders.push_back((*i)->Serialise());
	}

	file << toml::table{ {"Shaders", savedShaders} } << "\n\n";
	
	file << toml::table{ { "Camera", camera->Serialise() } } << "\n\n";

	//auto savedSceneObjects = toml::array();

	//TODO: add partBitMask to sceneObjects.
	//TODO: Make serialise function in SceneManager? Or is it Scene?
	//for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	//{
	//	savedSceneObjects.push_back((*i)->Serialise());
	//}
	
	//file << toml::table{ { "SceneObjects", savedSceneObjects } };

	file.close();
}

void Scene::Load()
{
	std::ifstream file("TestScene.toml");

	toml::table data = toml::parse(file);

	// TODO: Move most of this camera stuff to the camera class
	auto cam = data["Camera"];
	auto camPos = cam["position"].as_array();
	camera->position = Serialisation::LoadAsVec3(camPos);
	auto camRot = cam["rotation"].as_array();
	camera->yaw = camRot->at(0).value_or<float>(0.f);
	camera->pitch = camRot->at(1).value_or<float>(0.f);
	camera->UpdateVectors();
	

	auto loadingShaders = data["Shaders"].as_array();

	// TODO: Dont need to unload the like special shaders
	
	ResourceManager::UnloadShaders();

	for (int i = 0; i < loadingShaders->size(); i++)
	{
		shaders[i] = ResourceManager::LoadShader((loadingShaders->at(i)).as_table());

	}

	ResourceManager::RefreshAllMaterials();
	skybox->Refresh();
	ResourceManager::BindFlaggedVariables();

	//ResourceManager::

	//for (auto i = loadingShaders->begin(); i != loadingShaders->end(); i++)
	//{	
	//	ResourceManager::LoadShader((*i).as_table());
	//}


	//// TODO: Remove everything in the scene first
	//for (int i = 0; i < sceneObjects.size(); i++)
	//{
	//	delete sceneObjects[i];
	//}
	//sceneObjects.clear();

	if (!file.is_open()) {
		std::cout << "Error, save file to load not found\n";
	}



	file.close();

}
