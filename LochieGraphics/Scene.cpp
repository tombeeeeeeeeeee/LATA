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

#define LoadPart(container, savedName, type)                                                           \
toml::array* loading##type##s = data[##savedName].as_array();                                          \
for (int i = 0; i < loading##type##s->size(); i++)                                                     \
{                                                                                                      \
	toml::table* loading##type = loading##type##s->at(i).as_table();                                   \
	container[Serialisation::LoadAsUnsignedLongLong((*loading##type)["guid"])] = type(*loading##type); \
}



void Scene::Save()
{
	// TODO: Name is user selected, perhaps have a file opening dialog, see https://github.com/mlabbe/nativefiledialog
	// Or could just use an imgui pop up for the name alone and don't give the user a choice on the location / some limited explorer
	std::ofstream file("TestScene.toml");

	auto savedShaders = toml::array();
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		savedShaders.push_back((*i)->Serialise());
	}

	auto savedLights = toml::array();
	for (auto i = lights.begin(); i != lights.end(); i++)
	{
		savedLights.push_back((*i)->Serialise());
	}

	// TODO: Save Ecco & Sync
	
	file << toml::table{
		{ "WindowName", windowName},
		{ "Shaders", savedShaders},
		{ "Camera", camera->Serialise() },
		{ "Lights", savedLights},
	};
	file << "\n\n";

	file << SaveSceneObjectsAndParts();

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

	DeleteAllSceneObjects();
	LoadSceneObjectsAndParts(data);

	// TODO: Consider moving this
	gui.sceneObjectSelected = nullptr;


	if (!file.is_open()) {
		std::cout << "Error, save file to load not found\n";
	}

	file.close();
}

void Scene::DeleteAllSceneObjects()
{
	while (!sceneObjects.empty())
	{
		delete sceneObjects.begin()->second;
		sceneObjects.erase(sceneObjects.begin());
	}
}

SceneObject* Scene::FindSceneObjectOfName(std::string name)
{
	for (auto& i : sceneObjects)
	{
		if (i.second->name == name) {
			return i.second;
		}
	}
	return nullptr;
}

toml::table Scene::SaveSceneObjectsAndParts()
{
	auto savedSceneObjects = toml::array();
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		savedSceneObjects.push_back((*i).second->Serialise());
	}
	SavePart(renderers);
	SavePart(transforms);
	SavePart(animators);
	SavePart(rigidBodies);
	SavePart(colliders);
	SavePart(healths);

	return toml::table{
		{ "SceneObjects", savedSceneObjects },
		{ "Renderers", savedrenderers},
		{ "Transforms", savedtransforms},
		{ "Animators", savedanimators},
		{ "RigidBodies", savedrigidBodies},
		{ "Colliders", savedcolliders},
		{ "Healths", savedhealths},
		{ "Sync", sync->Serialise() },
		{ "Ecco", ecco->Serialise() },
	};
}

void Scene::LoadSceneObjectsAndParts(toml::table& data)
{
	toml::array* loadingSceneObjects = data["SceneObjects"].as_array();
	int loadingSceneObjectsSize = (int)loadingSceneObjects->size();
	for (int i = 0; i < loadingSceneObjectsSize; i++)
	{
		auto loadingSceneObject = loadingSceneObjects->at(i).as_table();
		new SceneObject(this, loadingSceneObject);
	}

	LoadPart(renderers, "Renderers", ModelRenderer);
	LoadPart(transforms, "Transforms", Transform);
	// Loading transforms doesn't keep the sceneobject pointer, they need to be refreshed
	for (auto& i : transforms)
	{
		i.second.so = sceneObjects[i.first];
	}
	LoadPart(animators, "Animators", Animator);
	LoadPart(rigidBodies, "RigidBodies", RigidBody);
	// TODO: Fix for colliders
	

	toml::array* loadingColliders = data["Colliders"].as_array(); 
	for (int i = 0; i < loadingColliders->size(); i++) {
		toml::table* loadingCollider = loadingColliders->at(i).as_table();
		// TODO: UNCOMMENT
		//colliders[Serialisation::LoadAsUnsignedLongLong((*loadingCollider)["guid"])] = Collider::Load(*loadingCollider);
	};
	
	LoadPart(healths, "Healths", Health);
	*ecco = Ecco(*data["Ecco"].as_table());
	*sync = Sync(*data["Sync"].as_table());

	// Load Hierarchy Data
	for (int i = 0; i < loadingSceneObjects->size(); i++)
	{
		auto loading = loadingSceneObjects->at(i).as_table();
		unsigned long long GUID = Serialisation::LoadAsUnsignedLongLong((*loading)["guid"]);
		SceneObject* sceneObject = sceneObjects[GUID];
		// TODO: Don't think sceneObjects need to store parents
		unsigned long long parentGUID = Serialisation::LoadAsUnsignedLongLong((*loading)["parent"]);
		if (parentGUID) {
			sceneObject->transform()->setParent(sceneObjects[parentGUID]->transform());
		}
		auto loadingChildren = (*loading)["children"].as_array();
		for (size_t i = 0; i < loadingChildren->size(); i++)
		{
			auto& temp = loadingChildren->at(i);
			unsigned long long childGUID = Serialisation::LoadAsUnsignedLongLong(temp);
			sceneObject->transform()->AddChild(sceneObjects[childGUID]->transform());
		}
	}
}
