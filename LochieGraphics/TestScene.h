#pragma once
#include "Scene.h"

#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "SceneObject.h"
#include "Light.h"
#include "MessengerInterface.h"

class TestScene : public Scene
{
private:
	MessengerInterface messengerInterface;

	Shader* litNormalShader = nullptr;
	Shader* litShader		= nullptr;
	Shader* lightCubeShader = nullptr;
	Shader* skyBoxShader	= nullptr;

	Mesh skybox;
	GLuint skyboxTexture;

	Model backpackModel;
	Model cubeModel;
	Model grassModel;
	Model soulSpearModel;
	Model testRedBoxModel;

	SceneObject* backpack   = new SceneObject();
	SceneObject* soulSpear  = new SceneObject();
	SceneObject* testRedBox = new SceneObject();
	SceneObject* grass      = new SceneObject();
	SceneObject* boxes      = new SceneObject();
	SceneObject* lightCube  = new SceneObject();

	PointLight pointLights[4] = {
		PointLight({ 0.05f, 0.05f, 0.05f }, { 1.0f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  3.2f,  2.0f }, 0.5f, 0.09f, 0.032f, 0),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, { -4.0f,  2.0f, -12.f }, 1.0f, 0.09f, 0.032f, 2),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight = DirectionalLight
		(          { 0.05f, 0.05f, 0.05f }, { 0.0f, 1.0f, 0.4f }, { 0.5f, 0.5f, 0.5f }, { -0.2f, -1.0f, -0.3f });

	Spotlight spotlight = 
		Spotlight( { 0.95f, 0.05f, 0.05f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f,  0.0f }, 0.5f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));

public:

	TestScene();
	void Start() override;
	void Update(float delta) override;
	void GUI() override;
	~TestScene() override;
};