#pragma once

#include "Transform.h"
#include "RigidBody.h"
#include "PointLight.h"
#include "Spotlight.h"
#include "DirectionalLight.h"

#include "RenderSystem.h"

#include "GUI.h"
#include "Audio.h"

#include <map>

class SceneObject;

class Scene
{
public:
	std::string windowName = "LATA";

	bool inPlay = false;
	bool lastFramePlayState = false;
	bool displayGUI = true;

	Camera* camera = nullptr;
	glm::vec2* cursorPos = nullptr;
	GUI gui;

	// TODO: Pretty sure these no longer need to be stored on the heap
	//std::vector<SceneObject*> sceneObjects = {};

	std::map<unsigned long long, SceneObject*> sceneObjects = {};
	std::unordered_map<unsigned long long, Transform> transforms = {};
	// TODO: There is types of animators, don't think its safe to store them like this
	std::unordered_map<unsigned long long, RigidBody> rigidBodies = {};
	std::unordered_map<unsigned long long, Collider*> colliders = {};
	std::unordered_map<unsigned long long, PointLight> pointLights = {};
	std::unordered_map<unsigned long long, Spotlight> spotlights = {};

	DirectionalLight directionalLight;

	std::vector<unsigned long long> markedForDeletion = {};

	std::vector<int> inputKeyWatch{};

	unsigned int* windowWidth = 0;
	unsigned int* windowHeight = 0;

	RenderSystem renderSystem;

	Audio audio;

	virtual void Start() {};
	virtual void Update(float delta) {};
	virtual void Draw(float delta) {};
	virtual void OnMouseDown() {};
	virtual void OnKeyDown(int key, int action) {};
	virtual void OnKey(int key, int action) {};

	virtual void GUI() {};

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
