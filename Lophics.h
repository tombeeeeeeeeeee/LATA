#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "SceneObject.h"

#include "Graphics.h"
#include "Maths.h"

#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>


//TODO: mtl files loaded with the obj
//TODO: should draw functions be const

class Lophics
{
	// Window
	static unsigned int windowWidth;
	static unsigned int windowHeight;
public:
	GLFWwindow* window;
private:
	// Camera
	static Camera camera;


	Texture* diffuseMap;
	Texture* specularMap;
	Texture* grass;


	Shader lightingShader;
	Shader simpleTexturedShader;
	Shader lightCubeShader;
	Shader screenShader;

	Shader shaders[4] = {
		lightingShader,
		simpleTexturedShader,
		lightCubeShader,
		screenShader
	};

	Mesh boxMesh;
	Mesh lightCubeMesh;
	Mesh quadMesh;
	Mesh testMesh;
	Mesh grassMesh;
	//Material boxMaterial;
	Model testLocModel;
	Model PointLightModel;
	SceneObject backpack;
	SceneObject pointLightScene;

	// Input
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void processInput(GLFWwindow* window);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static float lastX;
	static float lastY;
	static bool firstMouse;

	// Time
	static float deltaTime;
	float lastFrame = 0.0f;


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

		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  0.0f, -15.0f),
		glm::vec3(-1.5f, 0.0f, -3.5f),
		glm::vec3(-3.8f, 0.0f, -10.3f),
		glm::vec3(2.4f,  0.0f, -5.5f),
		glm::vec3(-1.7f, 0.0f, -9.5f),
		glm::vec3(1.3f,  0.0f, -3.5f),
		glm::vec3(1.5f,  0.0f, -3.5f),
		glm::vec3(1.5f,  0.0f, -4.5f),
		glm::vec3(-1.3f, 0.0f, -1.5f)
	};

	PointLight pointLights[4] = {
		PointLight({ 0.05f, 0.05f, 0.05f }, { 1.0f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 0.7f, 0.2f, 2.0f }, 1.0f, 0.09f, 0.032f),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { -4.0f, 2.0f, -12.0f }, 1.0f, 0.09f, 0.032f),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, -3.0f }, 1.0f, 0.09f, 0.032f)
	};
	DirectionalLight directionalLight =
		DirectionalLight({ 0.05f, 0.05f, 0.05f }, { 0.0f, 1.0f, 0.4f }, { 0.5f, 0.5f, 0.5f }, { -0.2f, -1.0f, -0.3f });

	SpotLight spotlight =
		SpotLight({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 0.1f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(10.5f)), glm::cos(glm::radians(17.0f)));


public:

	void Start();
	void Update();
	void Stop();
};

