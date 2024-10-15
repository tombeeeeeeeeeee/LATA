#include "Scene.h"

#include "SceneObject.h"
#include "Lights.h"
#include "Camera.h"
#include "Ecco.h"
#include "Sync.h"
#include "ResourceManager.h"
#include "ShaderEnum.h"
#include "Skybox.h"
#include "Collider.h"
#include "Collision.h"

#include "EditorGUI.h"
#include "Serialisation.h"
#include "Paths.h"

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

#define LoadPart(container, savedName, type)                                                               \
toml::array* loading##type##s = data[##savedName].as_array();                                              \
if(loading##type##s){																				       \
	for (int i = 0; i < loading##type##s->size(); i++)                                                     \
	{                                                                                                      \
		toml::table* loading##type = loading##type##s->at(i).as_table();                                   \
		container[Serialisation::LoadAsUnsignedLongLong((*loading##type)["guid"])] = type(*loading##type); \
	}																								       \
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

	// TODO: Should be a function
	//toml::parse_result parsed = toml::parse(file);
	//if (!parsed) {
	//	// TODO: Error
	//	std::cout << "Failed to load scene\n";
	//	file.close();
	//	return;
	//}
	//toml::table data = std::move(parsed).table();
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

void Scene::DeleteSceneObject(unsigned long long GUID)
{
	for (std::vector<unsigned long long>::iterator marks = markedForDeletion.begin(); marks != markedForDeletion.end(); marks++)
	{
		if (*marks == GUID) return;
	}
	markedForDeletion.push_back(GUID);
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

toml::table Scene::SaveSceneObjectsAndParts(bool(*shouldSave)(SceneObject*))
{
	auto savedSceneObjects = toml::array();
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		if (shouldSave) {
			if (!shouldSave(i->second)) {
				continue;
			}
		}
		savedSceneObjects.push_back((*i).second->Serialise());
	}
	SavePart(renderers);
	SavePart(transforms);
	SavePart(animators);
	SavePart(rigidBodies);
	SavePart(enemies);
	auto savedcolliders = toml::array(); for (auto i = colliders.begin(); i != colliders.end(); i++) {
		savedcolliders.push_back(i->second->Serialise(i->first));
	};
	SavePart(healths);
	SavePart(exits);

	return toml::table{
		{ "SceneObjects", savedSceneObjects },
		{ "Renderers", savedrenderers},
		{ "Transforms", savedtransforms},
		{ "Animators", savedanimators},
		{ "RigidBodies", savedrigidBodies},
		{ "Colliders", savedcolliders},
		{ "Enemies", savedenemies},
		{ "Healths", savedhealths},
		{ "Exits", savedexits},
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
	LoadPart(enemies, "Enemies", Enemy);
	// TODO: Fix for colliders
	

	toml::array* loadingColliders = data["Colliders"].as_array(); 
	for (int i = 0; i < loadingColliders->size(); i++) {
		toml::table* loadingCollider = loadingColliders->at(i).as_table();
		// TODO: UNCOMMENT
		colliders[Serialisation::LoadAsUnsignedLongLong((*loadingCollider)["guid"])] = Collider::Load(*loadingCollider);
	};
	
	LoadPart(healths, "Healths", Health);
	LoadPart(exits, "Exits", ExitElevator);

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

void Scene::InitialisePlayers()
{
	//ecco
	SceneObject* eccoSO = sceneObjects[ecco->GUID];
	if (!eccoSO->health()) eccoSO->setHealth(new Health());
	eccoSO->health()->currHealth = ecco->currHealth;
	eccoSO->health()->setMaxHealth(ecco->maxHealth);

	if (!eccoSO->rigidbody()) eccoSO->setRigidBody(new RigidBody(1.0f, 1.0f));
	if (eccoSO->rigidbody()->colliders.size() < 1)
	{
		eccoSO->rigidbody()->colliders.push_back(new PolygonCollider());
		eccoSO->rigidbody()->colliders.push_back(new PolygonCollider());
	}
	else if (eccoSO->rigidbody()->colliders.size() < 2)
	{
		eccoSO->rigidbody()->colliders.push_back(new PolygonCollider());
	}

	eccoSO->rigidbody()->colliders[0]->collisionLayer = (int)CollisionLayers::ecco;
	((PolygonCollider*)eccoSO->rigidbody()->colliders[0])->verts = {
			{75.0f,   75.0f},
			{75.0f,  -75.0f},
			{-75.0f, -75.0f},
			{-75.0f,  75.0f},
	};

	eccoSO->rigidbody()->colliders[1]->collisionLayer = (int)CollisionLayers::reflectiveSurface;
	((PolygonCollider*)eccoSO->rigidbody()->colliders[1])->verts = { {0.0f, 0.0f} };
	((PolygonCollider*)eccoSO->rigidbody()->colliders[1])->radius = 15.0f;

	eccoSO->rigidbody()->onCollision.push_back([this](Collision collision) { ecco->OnCollision(collision); });

	eccoSO->rigidbody()->vel = {0.0f, 0.0f};

	if (!eccoSO->renderer()) eccoSO->setRenderer(new ModelRenderer(ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_EccoRotated" + Paths::modelExtension),(unsigned long long) 0));
	else eccoSO->renderer()->model = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_EccoRotated" + Paths::modelExtension);
	eccoSO->renderer()->setMaterialTint({ 0.0f, 102.0f, 204.0f });
	//sync
	SceneObject* syncSO = sceneObjects[sync->GUID];
	if (!syncSO->health()) syncSO->setHealth(new Health());
	syncSO->health()->currHealth = sync->currHealth;
	syncSO->health()->setMaxHealth(sync->maxHealth);

	if (!syncSO->rigidbody()) syncSO->setRigidBody(new RigidBody(1.0f, 1.0f));
	if (syncSO->rigidbody()->colliders.size() < 1)
	{
		syncSO->rigidbody()->colliders.push_back(new PolygonCollider());
	}

	syncSO->rigidbody()->colliders[0]->collisionLayer = (int)CollisionLayers::sync;
	((PolygonCollider*)syncSO->rigidbody()->colliders[0])->verts = {
			{0.0f, 0.0f}
	};

	((PolygonCollider*)syncSO->rigidbody()->colliders[0])->radius = 40.0f;

	syncSO->rigidbody()->vel = { 0.0f, 0.0f };

	if (!syncSO->renderer()) syncSO->setRenderer(new ModelRenderer(ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_SyncBlockout_RevisedScale" + Paths::modelExtension), (unsigned long long) 0));
	else syncSO->renderer()->model = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_SyncBlockout_RevisedScale" + Paths::modelExtension);
	syncSO->renderer()->setMaterialTint({0.0f, 204.0f, 0.0f});
	sync->barrelOffset = { -75.0f, 70.0f, 5.0f };

	for (int i = 1; i < (int)CollisionLayers::count; i *= 2)
	{

		physicsSystem.SetCollisionLayerMask((int)CollisionLayers::reflectiveSurface, i, false);
		physicsSystem.SetCollisionLayerMask(i, i, false);
		switch (i)
		{
		case (int)CollisionLayers::base:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
				break;
		case (int)CollisionLayers::enemy:
			physicsSystem.SetCollisionLayerMask(i, i, true);
			break;
		case (int)CollisionLayers::syncProjectile:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::eccoProjectile, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::enemyProjectile, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			break;
		case (int)CollisionLayers::eccoProjectile:
				physicsSystem.SetCollisionLayerMask((int)CollisionLayers::enemyProjectile, i, false);
				physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
				physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
				break;
		case (int)CollisionLayers::enemyProjectile:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::softCover, i, false);
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			break;
		case (int)CollisionLayers::softCover:
			physicsSystem.SetCollisionLayerMask((int)CollisionLayers::halfCover, i, false);
			break;
		default:
			break;
		}
	}

	gameCamSystem.setCameraPositionDelta({-135, 35, -120});
	gameCamSystem.cameraZoomMinimum = 125.0f;
	gameCamSystem.cameraZoomMaximum = 205.0f;
	gameCamSystem.cameraZoomScale = 0.130f;
	gameCamSystem.cameraZoomOutSpeed = 0.330f;
	gameCamSystem.cameraZoomInSpeed = 0.01f;
	gameCamSystem.cameraMoveSpeed = 0.330f;
	gameCamSystem.viewAngle = {-135, 35, -120};
	gameCamSystem.setCameraPositionDelta({700, 1000, 700});
}
