#pragma once
#include "Scene.h"

#include "ResourceManager.h"
#include "Light.h"
#include "Skybox.h"
#include "MessengerInterface.h"
#include "Animator.h"
#include "FrameBuffer.h"
#include "BlendedAnimator.h"

class TestScene : public Scene
{
private:
	MessengerInterface messengerInterface;

	Shader* uiShader = nullptr;

	int skyboxIndex = 0;
	std::vector<Skybox*> skyboxes;

	Model* backpackModel;
	Model* cubeModel;
	Model* grassModel;
	Model* soulSpearModel;
	Model* testRedBoxModel;
	Model* xbotModel;
	Model* vampireModel;
	Model* tiresModel;


	SceneObject* backpack   = new SceneObject(this, "backpack   ");
	SceneObject* soulSpear  = new SceneObject(this, "soulSpear  ");
	SceneObject* testRedBox = new SceneObject(this, "testRedBox ");
	SceneObject* grass      = new SceneObject(this, "grass      ");
	SceneObject* boxes      = new SceneObject(this, "boxes      ");
	SceneObject* lightCube  = new SceneObject(this, "lightCube  ");
	SceneObject* xbot       = new SceneObject(this, "xbot       ");
	SceneObject* vampire    = new SceneObject(this, "vampire    ");
	SceneObject* tires      = new SceneObject(this, "tires      ");

	Animation xbotChicken;
	Animation xbotIdle;
	Animator xbotAnimator; // TODO: Make a better or new animator class that can go between two animations
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