#pragma once

#include "Scene.h"

#include "Camera.h"

#include "Graphics.h"

class SceneManager
{
	Scene* scene;

	// Window
	static unsigned int windowWidth;
	static unsigned int windowHeight;
	GLFWwindow* window;

	// Callbacks
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static void processInput(GLFWwindow* window);


	// Time
	static float deltaTime;
	float lastFrame = 0.0f;

	// Camera
	static Camera camera;
	static float lastX;
	static float lastY;
	static bool firstMouse;
	static bool lockedCamera;

	//static void processInput(GLFWwindow* window);
	
public:

	SceneManager(Scene* _scene);
	~SceneManager();

	SceneManager(const SceneManager& other) = delete;
	SceneManager& operator=(const SceneManager& other) = delete;

	void Update();
	bool ShouldClose() const;

};

