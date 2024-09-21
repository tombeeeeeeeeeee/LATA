#pragma once

#include "ModelRenderer.h"
#include "Transform.h"
#include "Animator.h"
#include "RigidBody.h"
#include "Health.h"
#include "Enemy.h"
#include "ExitElevator.h"

#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "GamePlayCameraSystem.h"
#include "HealthSystem.h"
#include "EnemySystem.h"

#include "GUI.h"

class SceneObject;
class Ecco;
class Sync;
class Skybox;

class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	Camera* camera = nullptr;
	glm::vec2* cursorPos = nullptr;
	GUI gui;

	Skybox* skybox = nullptr;

	// TODO: Pretty sure these no longer need to be stored on the heap
	//std::vector<SceneObject*> sceneObjects = {};

	std::unordered_map<unsigned long long, SceneObject*> sceneObjects = {};
	std::unordered_map<unsigned long long, ModelRenderer> renderers = {};
	std::unordered_map<unsigned long long, Transform> transforms = {};
	// TODO: There is types of animators, don't think its safe to store them like this
	std::unordered_map<unsigned long long, Animator> animators = {};
	std::unordered_map<unsigned long long, RigidBody> rigidBodies = {};
	std::unordered_map<unsigned long long, Collider*> colliders = {};
	std::unordered_map<unsigned long long, Health> healths = {};
	std::unordered_map<unsigned long long, Enemy> enemies = {};
	std::unordered_map<unsigned long long, ExitElevator> exits = {};
	std::vector<Light*> lights = {};

	std::vector<unsigned long long> markedForDeletion = {};

	Ecco* ecco = nullptr;
	Sync* sync = nullptr;


	std::vector<Shader*> shaders = {};

	std::vector<int> inputKeyWatch{};

	unsigned int* windowWidth;
	unsigned int* windowHeight;

	RenderSystem renderSystem;
	PhysicsSystem physicsSystem;
	GameplayCameraSystem gameCamSystem;
	HealthSystem healthSystem;
	EnemySystem enemySystem;

	virtual void Start() {};
	virtual void Update(float delta) {};
	virtual void Draw() {};
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
	virtual void Save();
	virtual void Load();

	void DeleteSceneObject(unsigned long long GUID);
protected:

	void DeleteAllSceneObjects();

	SceneObject* FindSceneObjectOfName(std::string name);

	toml::table SaveSceneObjectsAndParts();
	void LoadSceneObjectsAndParts(toml::table& data);
	void InitialisePlayers();
};
