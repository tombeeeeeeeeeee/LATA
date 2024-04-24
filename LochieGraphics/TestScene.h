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
	Shader* shadowDebug     = nullptr;

	Skybox* skybox;
	int skyboxIndex;
	std::vector<Skybox*> skyboxes;

	Model backpackModel;
	Model cubeModel;
	Model grassModel;
	Model soulSpearModel;
	Model testRedBoxModel;
	Model puppetModel;
	Model xbotModel;
	Model vampireModel;
	Model bottleModel;
	Model tiresModel;

	Mesh screenQuad;
	//unsigned int framebuffer; // TODO: Re imp framebuffer for post processing affects
	//unsigned int textureColorbuffer;
	//unsigned int rbo;

	FrameBuffer* shadowFrameBuffer = nullptr;
	GLuint depthMap;

	unsigned int quadVAO = 0; // TODO: I don't even think these need to be here anymore, remove
	unsigned int quadVBO = 0;

	bool showShadowDebug = false;

	SceneObject* backpack   = new SceneObject();
	SceneObject* soulSpear  = new SceneObject();
	SceneObject* testRedBox = new SceneObject();
	SceneObject* grass      = new SceneObject();
	SceneObject* boxes      = new SceneObject();
	SceneObject* lightCube  = new SceneObject();
	SceneObject* puppet     = new SceneObject();
	SceneObject* xbot       = new SceneObject();
	SceneObject* vampire    = new SceneObject();
	SceneObject* bottle     = new SceneObject();
	SceneObject* tires      = new SceneObject();

	Animation xbotChicken;
	Animator xbotAnimator;
	Animation vampireWalk;
	Animator vampireAnimator;
	Animation puppetAnimation;
	Animator puppetAnimator;

	PointLight pointLights[4] = {
		PointLight({ 0.00f, 0.00f, 0.00f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  5.2f,  2.0f }, 0.2f, 0.09f, 0.032f, 0),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, { -4.0f,  2.0f, -12.f }, 1.0f, 0.09f, 0.032f, 2),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight = DirectionalLight
		(          { 0.00f, 0.00f, 0.00f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { -0.533f, -0.533f, -0.533f });

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