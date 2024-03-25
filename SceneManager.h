#pragma once

#include "Scene.h"

#include "Camera.h"

#include "Graphics.h"

enum class WindowModes {
	windowed,
	fullscreenWindowed
};

class SceneManager
{
	Scene* scene;

	// Window
	static WindowModes windowMode;
	static unsigned int windowWidth;
	static unsigned int windowHeight;
	static GLFWwindow* window;

	// Callbacks //TODO: change how these functions are called, look at other examples on how callbacks can be handled
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void GLAPIENTRY ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void ProcessInput(GLFWwindow* window);
	static void ToggleFullscreen();

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

