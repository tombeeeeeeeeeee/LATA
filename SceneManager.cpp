#include "SceneManager.h"

#include "ResourceManager.h"

#include "imguiStuff.h"

#include <iostream>

unsigned int SceneManager::windowWidth = 1920 / 2;
unsigned int SceneManager::windowHeight = 1080 / 2;
WindowModes SceneManager::windowMode = WindowModes::windowed;
GLFWwindow* SceneManager::window = nullptr;


float SceneManager::deltaTime = 0.0f;

Camera SceneManager::camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float SceneManager::lastX = 600;
float SceneManager::lastY = 400;
bool SceneManager::firstMouse = true;
bool SceneManager::lockedCamera = false;


void SceneManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

void SceneManager::MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (lockedCamera) { return; }
	glfwSetCursorPos(window, 0, 0);
	if (firstMouse)
	{
		firstMouse = false;
		return;
	}


	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos;
	float yoffset = - ypos;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void SceneManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

void GLAPIENTRY SceneManager::ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// TODO: Write this function
}

void SceneManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_F11 && action == GLFW_PRESS) || (key == GLFW_KEY_ENTER && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)) {
		ToggleFullscreen();
	}

	// TODO: continue to lock camera when a mouse button is being held down too
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		if (!lockedCamera) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
			lockedCamera = true;
			firstMouse = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			lockedCamera = false;
		}
	}
}

void SceneManager::ProcessInput(GLFWwindow* window)
{
	if (ImGui::GetIO().WantCaptureKeyboard) { return; }

	// Camera movement
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	float cameraSpeed = 2.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera::UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera::DOWN, deltaTime);
	}	
}

void SceneManager::ToggleFullscreen()
{
	auto temp = glfwGetVideoMode(glfwGetPrimaryMonitor());
	firstMouse = true;
	switch (windowMode)
	{
	case WindowModes::windowed:
		windowMode = WindowModes::fullscreenWindowed;
		glfwSetWindowMonitor(SceneManager::window, glfwGetPrimaryMonitor(), 0, 0, temp->width, temp->height, temp->refreshRate);
		break;
	case WindowModes::fullscreenWindowed:
		windowMode = WindowModes::windowed;
		glfwSetWindowMonitor(window, nullptr, windowWidth / 4 , windowHeight / 4, windowWidth / 2, windowHeight / 2, temp->refreshRate);
		break;
	default:
		break;
	}
}

SceneManager::SceneManager(Scene* _scene) :
	scene(_scene)
{
	if (!glfwInit() || !_scene) {
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef DEBUG 
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(ErrorMessageCallback , nullptr);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

	//glfwGetPrimaryMonitor();
	//window = glfwCreateWindow(windowWidth, windowHeight, scene->windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
	window = glfwCreateWindow(windowWidth, windowHeight, scene->windowName.c_str(), nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		//TODO: something better than throw, same with loading GLAD and glfw
		throw;
	}

	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseMoveCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);

	glfwMakeContextCurrent(window);

	//TODO: Make function to change the input mode or something
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD\n";
		throw;
	}


	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsDark();

	//ImGui_ImplGlfw_InitForOpenGL(window, true);

	// Depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Face culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//// Stencil Testing
	/*glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);*/

	//TODO: Shaders, Meshes, Models, Textures should all be set up in the scene

	// Draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	scene->camera = &camera;

	scene->Start();

	std::cout << "Start finished\n";
}

SceneManager::~SceneManager()
{
	// De-allocate resources

	delete scene;

	// Textures
	ResourceManager::Unload();

	//TODO: unload meshes properly

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// Terminate and clear GLFW resources
	glfwTerminate();

	std::cout << "End\n";
}

void SceneManager::Update()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	ProcessInput(window);
	glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	scene->viewProjection = projection * view;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	scene->Update(deltaTime);
	scene->gui.Update();

	ImGui::Render();

	if (ImGui::GetDrawData())	//Render tends to get called once or twice before Update gets called, so we need to make sure this info exists.
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	// Check and call events and swap the buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool SceneManager::ShouldClose() const
{
	return glfwWindowShouldClose(window);
}
