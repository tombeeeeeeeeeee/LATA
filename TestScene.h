#pragma once
#include "Scene.h"

#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "SceneObject.h"
#include "Light.h"

class TestScene : public Scene
{
private:
	Shader* lightingShader;
	Shader* lightCubeShader;

	Model backpackModel;
	Model boxModel;
	Model grassModel;
	Model lightCubeModel;
	SceneObject backpack;
	SceneObject grass;
	SceneObject box;
	SceneObject lightCube;

	glm::vec3 cubePositions[10] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 grassPositions[10] = {

		glm::vec3( 0.0f, 0.0f,  00.0f),
		glm::vec3( 2.0f, 0.0f, -15.0f),
		glm::vec3(-1.5f, 0.0f, -03.5f),
		glm::vec3(-3.8f, 0.0f, -10.3f),
		glm::vec3( 2.4f, 0.0f, -05.5f),
		glm::vec3(-1.7f, 0.0f, -09.5f),
		glm::vec3( 1.3f, 0.0f, -03.5f),
		glm::vec3( 1.5f, 0.0f, -03.5f),
		glm::vec3( 1.5f, 0.0f, -04.5f),
		glm::vec3(-1.3f, 0.0f, -01.5f)
	};

	PointLight pointLights[4] = {
		PointLight({ 0.05f, 0.05f, 0.05f }, { 1.0f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 0.7f, 0.2f, 2.0f }, 1.0f, 0.09f, 0.032f, 0),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f, 1),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { -4.0f, 2.0f, -12.0f }, 1.0f, 0.09f, 0.032f, 2),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, -3.0f }, 1.0f, 0.09f, 0.032f, 3)
	};
	DirectionalLight directionalLight =
		DirectionalLight({ 0.05f, 0.05f, 0.05f }, { 0.0f, 1.0f, 0.4f }, { 0.5f, 0.5f, 0.5f }, { -0.2f, -1.0f, -0.3f });

	Spotlight spotlight = Spotlight({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 0.5f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));
	//Spotlight spotlight = Spotlight({ 0.0f, 00.f, 00.f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.00f, 0.000f, { 0.0f, 0.0f, 0.0f }, 0.f, 0.f);

public:

	void Start() override;
	void Update(float delta) override;
	~TestScene() override;
};

