#pragma once
#include "Scene.h"

#include "Lights.h"
#include "MessengerInterface.h"
#include "Animator.h"
#include "BlendedAnimator.h"
#include "Animation.h"

class TestScene : public Scene
{
private:
	MessengerInterface messengerInterface;

	Shader* uiShader = nullptr;

	int skyboxIndex = 0;
	std::vector<Skybox*> skyboxes;

	SceneObject* backpack;
	SceneObject* soulSpear;
	SceneObject* testRedBox;
	SceneObject* grass;
	SceneObject* boxes;
	SceneObject* lightCube;
	SceneObject* xbot;
	SceneObject* vampire;
	SceneObject* aniTest;
	SceneObject* tires;

	Animation xbotChicken;
	Animation xbotIdle;
	Animator xbotAnimator;
	Animator xbotOtherAnimator;
	BlendedAnimator xbotBlendedAnimator;
	float lerpAmount = 0.f;

	Animation vampireWalk;
	Animator vampireAnimator;

	PointLight pointLights[4] = {
		PointLight({ 1.0f, 1.0f, 1.0f }, {  0.0f,  5.2f,  2.0f }, 0.2f, 0.09f, 0.032f, 0),
		PointLight({ 0.8f, 0.8f, 0.8f }, {  2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
		PointLight({ 0.8f, 0.8f, 0.8f }, { -4.0f,  2.0f, -12.f }, 1.0f, 0.09f, 0.032f, 2),
		PointLight({ 0.8f, 0.8f, 0.8f }, {  0.0f,  0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight = DirectionalLight
		(          { 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });

	Spotlight spotlight = 
		Spotlight({ 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f,  0.0f }, 0.5f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));

public:

	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
	void OnMouseDown() override;
	void GUI() override;
	void OnWindowResize() override;

	~TestScene() override;
};