#pragma once

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
#include "Bollard.h"
#include "Triggerable.h"
#include "PointLight.h"
#include "Spotlight.h"
#include "DirectionalLight.h"

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
class Ecco;
class Sync;
class Skybox;

class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	
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
	std::unordered_map<unsigned long long, ModelRenderer> renderers = {};
	std::unordered_map<unsigned long long, Transform> transforms = {};
	// TODO: There is types of animators, don't think its safe to store them like this
	std::unordered_map<unsigned long long, Animator> animators = {};
	std::unordered_map<unsigned long long, RigidBody> rigidBodies = {};
	std::unordered_map<unsigned long long, Collider*> colliders = {};
	std::unordered_map<unsigned long long, Health> healths = {};
	std::unordered_map<unsigned long long, Enemy> enemies = {};
	std::unordered_map<unsigned long long, ExitElevator> exits = {};
	std::unordered_map<unsigned long long, SpawnManager> spawnManagers = {};
	std::unordered_map<unsigned long long, PressurePlate> plates = {};
	std::unordered_map<unsigned long long, Door> doors = {};
	std::unordered_map<unsigned long long, Bollard> bollards = {};
	std::unordered_map<unsigned long long, Triggerable> triggerables = {};
	std::unordered_map<unsigned long long, PointLight> pointLights = {};
	std::unordered_map<unsigned long long, Spotlight> spotlights = {};

	DirectionalLight directionalLight;

	std::vector<unsigned long long> markedForDeletion = {};

	Ecco* ecco = nullptr;
	Sync* sync = nullptr;
	bool eccoHealPressed = false;
	bool syncHealPressed = false;

	// TODO: Should probably be apart of the resource manager tbh
	std::vector<Shader*> shaders = {};

	std::vector<int> inputKeyWatch{};

	unsigned int* windowWidth = 0;
	unsigned int* windowHeight = 0;

	RenderSystem renderSystem;
	PhysicsSystem physicsSystem;
	GameplayCameraSystem gameCamSystem;
	HealthSystem healthSystem;
	EnemySystem enemySystem;
	TriggerSystem triggerSystem;
	DoorAndBollardSystem dabSystem;
	ParticleSystem particleSystem;
	Audio audio;

	virtual void Start() {};
	virtual void Update(float delta) {};
	virtual void Draw(float delta) {};
	virtual void OnMouseDown() {};
	virtual void OnKeyDown(int key, int action) {};
	virtual void OnKey(int key, int action) {};

	// TODO: Maybe move some of the GUI code that is in the GUI class here
	void BaseGUI();
	virtual void GUI() {};

	virtual void OnWindowResize() {};

	Scene();
	virtual ~Scene();

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	// These get called when the menu button gets pressed, do not have to be directly responsible for saving
	virtual void Save() {};
	virtual void Load() {};

	void DeleteSceneObject(unsigned long long GUID);
protected:

	void DeleteAllSceneObjectsAndParts();

	SceneObject* FindSceneObjectOfName(std::string name);

	toml::table SaveSceneObjectsAndParts(bool(*shouldSave)(SceneObject*) = nullptr);
	void LoadSceneObjectsAndParts(toml::table& data);
	void EnsureAllPartsHaveSceneObject();
	void InitialisePlayers();
};
