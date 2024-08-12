#pragma once

#include "SceneObject.h"
#include "Camera.h"
#include "Light.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "Ecco.h"
#include "shaderEnum.h"

#include "Maths.h"
#include "GUI.h"

#include <string>
#include "RenderSystem.h"


class Scene
{
public:
	std::string windowName = "Lochie's Scene";

	Camera* camera = nullptr;
	glm::vec2* cursorPos = nullptr;
	GUI gui;

	Skybox* skybox = nullptr;

	std::vector<SceneObject*> sceneObjects = {};

	std::unordered_map<unsigned long long, ModelRenderer> renderers = {};
	std::unordered_map<unsigned long long, Transform> transforms = {};
	std::unordered_map<unsigned long long, Animator> animators = {};
	std::unordered_map<unsigned long long, RigidBody> rigidBodies = {};
	std::unordered_map<unsigned long long, Collider> colliders = {};
	std::vector<Light*> lights = {};

	Ecco* ecco = nullptr;

	std::vector<Shader*> shaders = {};

	std::vector<int> inputKeyWatch{};

	unsigned int* windowWidth;
	unsigned int* windowHeight;

	RenderSystem* renderSystem;

	virtual void Start() {};
	virtual void EarlyUpdate() {};
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

	void Save();
	void Load();
};
