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

class TestScene : public Scene
{
private:
	MessengerInterface messengerInterface;

	Shader* litNormalShader = nullptr;
	Shader* litShader		= nullptr;
	Shader* lightCubeShader = nullptr;
	Shader* skyBoxShader    = nullptr;
	Shader* animateShader   = nullptr;
	Shader* pbrShader       = nullptr;
	Shader* screenShader    = nullptr;
	Shader* shadowMapDepth  = nullptr;
	Shader* shadowMapping   = nullptr;

	Skybox* skybox;

	Model backpackModel;
	Model cubeModel;
	Model grassModel;
	Model soulSpearModel;
	Model testRedBoxModel;
	Model puppetModel;
	Model xbotModel;
	Model vampireModel;

	Mesh screenQuad;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;

	unsigned int depthMapFBO;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMap;


	SceneObject* backpack   = new SceneObject();
	SceneObject* soulSpear  = new SceneObject();
	SceneObject* testRedBox = new SceneObject();
	SceneObject* grass      = new SceneObject();
	SceneObject* boxes      = new SceneObject();
	SceneObject* lightCube  = new SceneObject();
	SceneObject* puppet     = new SceneObject();
	SceneObject* xbot       = new SceneObject();
	SceneObject* vampire    = new SceneObject();

	Animation xbotChicken;
	Animator xbotAnimator;
	Animation vampireWalk;
	Animator vampireAnimator;
	Animation puppetAnimation;
	Animator puppetAnimator;

	PointLight pointLights[4] = {
		PointLight({ 0.05f, 0.05f, 0.05f }, { 1.0f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  3.2f,  2.0f }, 0.5f, 0.09f, 0.032f, 0),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, { -4.0f,  2.0f, -12.f }, 1.0f, 0.09f, 0.032f, 2),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight = DirectionalLight
		(          { 0.05f, 0.05f, 0.05f }, { 0.4f, 0.4f, 0.4f }, { 0.5f, 0.5f, 0.5f }, { -0.2f, -1.0f, -0.3f });

	Spotlight spotlight = 
		Spotlight( { 0.05f, 0.05f, 0.05f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f,  0.0f }, 0.5f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));

public:

	TestScene();
	void Start() override;
	void EarlyUpdate() override;
	void Update(float delta) override;
	void Draw() override;
	void GUI() override;

	void OnWindowResize() override;

	~TestScene() override;

private:

	void LoadRenderBuffer();
};