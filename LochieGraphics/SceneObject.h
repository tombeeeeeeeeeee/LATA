#pragma once
#include "Serialisation.h"

#include "ModelRenderer.h"
#include "Animator.h"
#include "Light.h"
#include "Transform.h"
#include "Scene.h"

#include "Maths.h"

#include <vector>
#include <string>

class Shader;

// TODO: Make version of the SceneObject that draws the same model in multiple spots, using instancing
// TODO: Look into how instancing should be handled

//TODO: Add GetPart Functions
enum Parts
{
	transform = 1 << 0,
	modelRenderer = 1 << 1,
	animator = 1 << 2,
	rigidBody = 1 << 3,
	light = 1 << 4,
};

class Scene;

class SceneObject
{
public:

	std::string name = "Unnamed Scene Object";
	unsigned long long GUID = 0;
	unsigned int parts = 0;
	Scene* scene;

	//TODO: Ask lochy how to get a GUID
	SceneObject(Scene* _scene);
	SceneObject(Scene* scene, glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	~SceneObject();

	SceneObject(const SceneObject& other) = delete;
	SceneObject& operator=(const SceneObject& other) = delete;

	void Update(float delta);
	void Draw(Shader* override = nullptr) const;
	void GUI();
};



