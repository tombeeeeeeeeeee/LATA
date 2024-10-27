#pragma once

#include "Graphics.h"

#include "FixedSizeQueue.h"

#include "Camera.h"

class Skybox;
class Scene;

// TODO: fullscreen window with border, exclusive fullscreen
enum class WindowModes {
	windowed,
	borderlessFullscreen,
	maximised,
};

class SceneManager
{
private:

	// Camera
	static float lastX;
	static float lastY;
	static bool firstMouse;
	static bool lockedCamera;

	static glm::vec2 cursorPos;

	// Time
	static float deltaTime;
	float lastFrame = 0.0f;
public:
	static Camera camera;
	static FixedSizeQueue<float, 300> frameTimes;

	static GLFWwindow* window;
	static Scene* scene;

	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::mat4 viewProjection;

private:
	// TODO: Perhaps variables that need to be accessed by both the scene and scene manager class should be in a seperate class (such as these window height/width)

	// Window
	static WindowModes windowMode;
	static unsigned int windowWidth;
	static unsigned int windowHeight;

	// TODO: Should prob be static
	Skybox* defaultSkybox = nullptr;

	// Callbacks //TODO: change how these functions are called, look at other examples on how callbacks can be handled
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void GLAPIENTRY ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static void RefreshInputMode();


	static void ProcessInput(GLFWwindow* window);
	static void ProcessKeyboardInput(GLFWwindow* window);
	static void ProcessMouseInput(GLFWwindow* window);
	static void ToggleFullscreen(); // TODO: fullscreen for correct monitor and remember size and pos
	
public:

	SceneManager(Scene* _scene);
	~SceneManager();

	SceneManager(const SceneManager& other) = delete;
	SceneManager& operator=(const SceneManager& other) = delete;

	void Update();
	bool ShouldClose() const;

	static void SwitchToWindowMode(WindowModes mode);
};

