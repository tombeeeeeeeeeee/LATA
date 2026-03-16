#pragma once

#include "PartsList.h"
#include "ModelRenderer.h"
#include "Transform.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Health.h"
#include "Enemy.h"
#include "ExitElevator.h"
#include "SpawnManager.h"
#include "PressurePlate.h"
#include "Door.h"
#include "Decal.h"
#include "Bollard.h"
#include "Triggerable.h"
#include "PointLight.h"
#include "Spotlight.h"
#include "DirectionalLight.h"
#include "InvisibleWall.h"
#include "Sync.h"
#include "Ecco.h"

#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "GamePlayCameraSystem.h"
#include "HealthSystem.h"
#include "EnemySystem.h"
#include "TriggerSystem.h"
#include "DoorAndBollardSystem.h"
#include "ParticleSystem.h"
#include "GUI.h"
#include "Audio.h"

#include <map>

class SceneObject;
class Skybox;

class Scene
{
public:
	std::string windowName = "Sync & Ecco";

	
	bool inPlay = false;
	bool lastFramePlayState = false;
	bool displayGUI = true;

	Camera* camera = nullptr;
	glm::vec2* cursorPos = nullptr;
	GUI gui;

	Skybox* skybox = nullptr;

	// TODO: Pretty sure these no longer need to be stored on the heap
	//std::vector<SceneObject*> sceneObjects = {};



	std::map<unsigned long long, SceneObject*> sceneObjects = {};

#define PART_ENTRY(index, lower, cls, container, access, store, ignore, ...) \
std::unordered_map<unsigned long long, cls store> container = {};

	ALL_PARTS

#undef PART_ENTRY

	std::unordered_map<unsigned long long, Transform> transforms = {};
	
	DirectionalLight directionalLight;

	std::vector<unsigned long long> markedForDeletion = {};

	bool eccoHealPressed = false;
	bool syncHealPressed = false;
	SoLoud::handle eccoCurrHandle;
	SoLoud::handle syncCurrHandle;

	std::vector<int> inputKeyWatch{};

	unsigned int* windowWidth = 0;
	unsigned int* windowHeight = 0;

	RenderSystem renderSystem { gui };
	PhysicsSystem physicsSystem;
	GameplayCameraSystem gameCamSystem;
	HealthSystem healthSystem = { gui };
	EnemySystem enemySystem = { gui };
	TriggerSystem triggerSystem;
	DoorAndBollardSystem dabSystem;
	ParticleSystem particleSystem;
	Audio audio;
	Input input{ gui };

	virtual void Start() {};
	virtual void Update(float delta) {};
	virtual void Draw(float delta) {};
	virtual void OnMouseDown() {};
	virtual void OnKeyDown(int key, int action) {};
	virtual void OnKey(int key, int action) {};

	virtual void GUI() {};
	bool doSteering = true;
	bool doCollisions = true;
	bool doLOS = true;
	bool doBroadphase = true;
	bool doAnimations = true;

	virtual void OnWindowResize() {};

	Scene();
	virtual ~Scene();

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	// These get called when the menu button gets pressed, do not have to be directly responsible for saving
	virtual void Save() {};
	virtual void Load() {};

	void DeleteSceneObjectAndChildren(unsigned long long GUID);
	void DeleteSceneObjectKeepChildren(unsigned long long GUID);
	void DeleteSceneObjectsMarkedForDelete();
protected:

	void DeleteAllSceneObjectsAndParts();

	SceneObject* FindSceneObjectOfName(std::string name);

	toml::table SaveSceneObjectsAndParts(bool(*shouldSave)(SceneObject*) = nullptr);
	void LoadSceneObjectsAndParts(toml::table& data);
	void EnsureAllPartsHaveSceneObject();
	void EnsurePartsValueMatchesParts();
};
