#pragma once
#include "Scene.h"

#include "PhysicsSystem.h"

#include "Input.h"

class GameTest : public Scene
{
private:

	Input input;

	PhysicsSystem physicsSystem;

	SceneObject* h = new SceneObject(this);
	SceneObject* r = new SceneObject(this);

	std::unordered_map<unsigned long long, RigidBody> rigidbodies = {};

	RigidBody* hRb;
	RigidBody* rRb;

	float hRadius = 0.1f;
	float rRadius = 0.1f;

	glm::vec2 wheelDirection = {0.0f, 0.0f};
	float rot = 0.0f;

public:

	PointLight pointLights[4] = {
	PointLight({ 1.0f, 1.0f, 1.0f }, {  0.0f,  5.2f,  2.0f }, 0.2f, 0.09f, 0.032f, 0),
	PointLight({ 0.8f, 0.8f, 0.8f }, {  2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
	PointLight({ 0.8f, 0.8f, 0.8f }, { -4.0f,  2.0f, -12.f }, 1.0f, 0.09f, 0.032f, 2),
	PointLight({ 0.8f, 0.8f, 0.8f }, {  0.0f,  0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });

	Spotlight spotlight =
		Spotlight({ 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f,  0.0f }, 0.5f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));



	GameTest();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
	//	void OnMouseDown() override;
	void GUI() override;
	//	void OnWindowResize() override;
	//
	~GameTest() override;

};

