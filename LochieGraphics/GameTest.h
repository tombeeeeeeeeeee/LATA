#pragma once
#include "Scene.h"

#include "PhysicsSystem.h"
#include "EnemySystem.h"
#include "GamePlayCameraSystem.h"

#include "Input.h"

#include "Image.h"
#include <array>


class GameTest : public Scene
{
private:

	Input input;

	PhysicsSystem physicsSystem;
	GameplayCameraSystem gameCamSystem;
	EnemySystem enemySystem;

	SceneObject* h = new SceneObject(this);
	SceneObject* r = new SceneObject(this);

	RigidBody* hRb = nullptr;
	RigidBody* rRb = nullptr;

	float hRadius = 0.1f;
	float rRadius = 0.1f;


	float rot = 0.0f;

	Image level;

	float halfGridSpacing = 0.5f;

	bool MapCellIs(unsigned char* cell, unsigned char r, unsigned char g, unsigned char b);

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

private: 

	bool firstFrame = true;

	bool targettingP1 = false;
	bool singlePlayerMode = false;
	float singlePlayerZoom = 0.5f;

	std::array<std::string, 6> irradianceFaces = {
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png"
	};

};

