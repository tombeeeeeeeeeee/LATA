#pragma once

#include "glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#define GLM_FORCE_XYZW_ONLY 1
#include "Maths.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"


#include "Shader.h"

#include "Camera.h"

#include "Light.h"

#include "mesh.h"

#include "Material.h"

#include "Model.h"


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


	Shader lightingShader;
	Shader lightCubeShader;

	Mesh cubeMesh;
	Mesh quadMesh;
	Mesh testMesh;
	Material boxMaterial;
	Model testModel;

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

	PointLight pointLights[4] = {
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 0.7f, 0.2f, 2.0f }, 1.0f, 0.09f, 0.032f),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 2.3f, -3.3f, -4.0f }, 1.0f, 0.09f, 0.032f),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { -4.0f, 2.0f, -12.0f }, 1.0f, 0.09f, 0.032f),
		PointLight({ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, -3.0f }, 1.0f, 0.09f, 0.032f)
	};
	DirectionalLight directionalLight =
		DirectionalLight({ 0.05f, 0.05f, 0.05f }, { 1.0f, 0.4f, 0.4f }, { 0.5f, 0.5f, 0.5f }, { -0.2f, -1.0f, -0.3f });

	SpotLight spotLight =
		SpotLight({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.0f, 0.09f, 0.032f, { 0.0f, 0.0f, 0.0f }, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));


public:

	void Start();
	void Update();
	void Stop();
};

