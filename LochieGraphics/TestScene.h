#pragma once
#include "Scene.h"

#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "SceneObject.h"
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

	Shader* screenShader    = nullptr;
	Shader* shadowMapDepth  = nullptr;
	Shader* shadowMapping   = nullptr;
	Shader* shadowDebug     = nullptr;
	Shader* superShader     = nullptr;
	Shader* simpleTextured  = nullptr;
	Shader* uiShader        = nullptr;
	Shader* hdrBloom		= nullptr;

	Skybox* skybox = nullptr;
	int skyboxIndex = 0;
	std::vector<Skybox*> skyboxes;

	Model backpackModel;
	Model cubeModel;
	Model grassModel;
	Model soulSpearModel;
	Model testRedBoxModel;
	Model xbotModel;
	Model vampireModel;
	Model tiresModel;


	SceneObject* backpack   = new SceneObject(this);
	SceneObject* soulSpear  = new SceneObject(this);
	SceneObject* testRedBox = new SceneObject(this);
	SceneObject* grass      = new SceneObject(this);
	SceneObject* boxes      = new SceneObject(this);
	SceneObject* lightCube  = new SceneObject(this);
	SceneObject* puppet     = new SceneObject(this);
	SceneObject* xbot       = new SceneObject(this);
	SceneObject* vampire    = new SceneObject(this);
	SceneObject* bottle     = new SceneObject(this);
	SceneObject* tires      = new SceneObject(this);

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
	void EarlyUpdate() override;
	void Update(float delta) override;
	void Draw() override;
	void OnMouseDown() override;
	void GUI() override;
	void UpdateAllTransforms();
	void OnWindowResize() override;

	~TestScene() override;
};