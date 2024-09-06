#include "Scene.h"

#include "ResourceManager.h"

#include "EditorGUI.h"

#include <fstream>
#include <iostream>

void Scene::BaseGUI()
{

}

Scene::Scene()
{
	gui.scene = this;
	ecco = new Ecco();
	sync = new Sync();
}

Scene::~Scene()
{
	//TODO: clear sceneobjects.
	//for (int i = 0; i < sceneObjects.size(); i++)
	//{
	//	delete sceneObjects[i];
	//}
}

#define SavePart(container)                                        \
	auto saved##container = toml::array();                         \
	for (auto i = container.begin(); i != container.end(); i++)    \
	{                                                              \
		saved##container.push_back(i->second.Serialise(i->first)); \
	}

void Scene::Save() const
{
	// TODO: Name is user selected, perhaps have a file opening dialog, see https://github.com/mlabbe/nativefiledialog
	// Or could just use an imgui pop up for the name alone and don't give the user a choice on the location / some limited explorer
	std::ofstream file("TestScene.toml");

	auto savedShaders = toml::array();
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		savedShaders.push_back((*i)->Serialise());
	}

	auto savedSceneObjects = toml::array();
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		savedSceneObjects.push_back((*i).second->Serialise());
	}

	auto savedLights = toml::array();
	for (auto i = lights.begin(); i != lights.end(); i++)
	{
		savedLights.push_back((*i)->Serialise());
	}

	SavePart(renderers);
	SavePart(transforms);
	SavePart(animators);
	SavePart(rigidBodies);
	//SavePart(colliders);
	SavePart(healths);

	// TODO: Save Ecco & Sync
	
	file << toml::table{
		{ "WindowName", windowName},
		{ "Shaders", savedShaders},
		{ "Camera", camera->Serialise() },
		{ "SceneObjects", savedSceneObjects },
		{ "Lights", savedLights},
		{ "Renderers", savedrenderers},
		{ "Transforms", savedtransforms},
		{ "Animators", savedanimators},
		{ "RigidBodies", savedrigidBodies},
		//{ "Colliders", savedcolliders},
		{ "Healths", savedhealths},
	};

	// TODO: Save RenderSystem stuff

	file.close();
}

void Scene::Load()
{
	std::ifstream file("TestScene.toml");

	toml::table data = toml::parse(file);

	// TODO: Move most of this camera stuff to the camera class
	auto cam = data["Camera"];
	auto camPos = cam["position"].as_array();
	//camera->transform. = Serialisation::LoadAsVec3(camPos);
	//camera->position = { camPos->at(0).value_or<float>(0.0f), camPos->at(1).value_or<float>(0.f), camPos->at(2).value<float>().value()};
	auto camRot = cam["rotation"].as_array();


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

	sceneObjects.clear();

	auto loadingSceneObjects = data["SceneObjects"].as_array();
	for (int i = 0; i < loadingSceneObjects->size(); i++)
	{
		auto loadingSceneObject = loadingSceneObjects->at(i).as_table();
		new SceneObject(this, loadingSceneObject);
	}

	auto loadingTransforms = data["Transforms"].as_array();
	for (int i = 0; i < loadingTransforms->size(); i++)
	{
		auto loadingTransform = loadingTransforms->at(i).as_table();

		transforms[Serialisation::LoadAsUnsignedLongLong((*loadingTransform)["guid"])] = Transform(*loadingTransform);
	}

	// TODO: Consider moving this
	gui.sceneObjectSelected = nullptr;


	if (!file.is_open()) {
		std::cout << "Error, save file to load not found\n";
	}



	file.close();

}
