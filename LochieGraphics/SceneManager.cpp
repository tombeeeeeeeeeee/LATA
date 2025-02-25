#include "SceneManager.h"

#include "Scene.h"
#include "ResourceManager.h"
#include "SceneObject.h"
#include "Lights.h"
#include "UserPreferences.h"
#include "PrefabManager.h"
#include "Paths.h"

// This includes imgui differently then other files as it is managed here
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "ImGuiStyles.h"

#include <iostream>
#include <array>
#include <filesystem>
#include <fstream>

unsigned int SceneManager::windowWidth = 1920 / 2;
unsigned int SceneManager::windowHeight = 1080 / 2;
WindowModes SceneManager::windowMode = WindowModes::windowed;
GLFWwindow* SceneManager::window = nullptr;
Scene* SceneManager::scene = nullptr;
glm::vec2 SceneManager::cursorPos = { 0, 0 };

float SceneManager::deltaTime = 0.0f;
FixedSizeQueue<float, 300> SceneManager::frameTimes;
float SceneManager::maxFrameTime = 0.018f;

Camera SceneManager::camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float SceneManager::lastX = 600;
float SceneManager::lastY = 400;
bool SceneManager::firstMouse = true;
bool SceneManager::lockedCamera = true; // TODO: better names for these variables that change input mode

glm::mat4 SceneManager::view = {};
glm::mat4 SceneManager::projection = {};
glm::mat4 SceneManager::viewProjection = {};

SceneManager::SceneManager(Scene* _scene)
{
	scene = _scene;
	if (!glfwInit() || !_scene) {
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if _DEBUG  
	std::cout << "Running in debug mode!\n";
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	//glfwGetPrimaryMonitor();
	//window = glfwCreateWindow(windowWidth, windowHeight, scene->windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
	window = glfwCreateWindow(windowWidth, windowHeight, scene->windowName.c_str(), nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		throw;
	}

	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseMoveCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	glfwMakeContextCurrent(window);
	
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD\n";
		throw;
	}

#if _DEBUG  
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(ErrorMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(); // TODO: Theres an overload for this that takes a version, see if should be using

	auto& io = ImGui::GetIO();
	std::string fontExtension = ".ttf";
	for (auto& i : std::filesystem::directory_iterator(Paths::fontsPath))
	{
		std::string path = i.path().string();
		if (path.size() < fontExtension.size()) { continue; }
		if (path.compare(path.size() - fontExtension.size(), fontExtension.size(), fontExtension) == 0) {
			io.Fonts->AddFontFromFileTTF(path.c_str(), 13);
		}
	}

	io.Fonts->AddFontDefault();

	// Depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Face culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	scene->renderSystem.window = window;

	camera.nearPlane = 0.5f;
	camera.farPlane = 100.0f;
	scene->camera = &camera;

	scene->windowWidth = &windowWidth;
	scene->windowHeight = &windowHeight;

	scene->cursorPos = &cursorPos;

	std::cout << "Loading main Shaders...\n";
	ResourceManager::shadowMapDepth = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "simpleDepthShader" + Paths::shaderExtension);
	ResourceManager::shadowDebug    = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "shadowDebug"       + Paths::shaderExtension);
	ResourceManager::screen         = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "Display"           + Paths::shaderExtension);
	ResourceManager::brdf           = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "brdf"              + Paths::shaderExtension);
	ResourceManager::prefilter      = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "prefilter"         + Paths::shaderExtension);
	ResourceManager::downSample     = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "downSample"        + Paths::shaderExtension);
	ResourceManager::upSample       = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "upSample"          + Paths::shaderExtension);
	ResourceManager::irradiance     = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "irradiance"        + Paths::shaderExtension);
	ResourceManager::lines          = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "lineRenderer"      + Paths::shaderExtension);
	ResourceManager::ssao           = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "ssao"              + Paths::shaderExtension);
	ResourceManager::ssaoBlur       = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "ssaoBlur"          + Paths::shaderExtension);
	ResourceManager::prepass        = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "prepass"           + Paths::shaderExtension);
	ResourceManager::super          = ResourceManager::LoadShaderAsset(Paths::shadersSaveLocation + "superDuper"        + Paths::shaderExtension);
	std::cout << "Loaded main Shaders\n";

	std::cout << "Loading Texture Assets...\n";
	for (auto& i : std::filesystem::directory_iterator(Paths::textureSaveLocation)) {
		ResourceManager::LoadTextureAsset(i.path().string());
	}
	std::cout << "Loaded Texture Assets\n";

	std::cout << "Loading Material Assets...\n";
	for (auto& i : std::filesystem::directory_iterator(Paths::materialSaveLocation)) {
		ResourceManager::LoadMaterialAsset(i.path().string());
	}
	std::cout << "Loaded Material Assets\n";

	std::cout << "Loading Model Assets...\n";
	
	std::cout << "Loaded Model Assets\n";

	std::cout << "Loading Animation Assets...\n";

	std::cout << "Loaded Animations\n";

	//std::array<std::string, 6> skyboxFaces = { "images/skybox/left.jpg", "images/skybox/right.jpg", "images/skybox/top.jpg", "images/skybox/bottom.jpg", "images/skybox/front.jpg", "images/skybox/back.jpg" };
	std::array<std::string, 6> skyboxFaces = { "images/skybox/black.png", "images/skybox/black.png", "images/skybox/black.png", "images/skybox/black.png", "images/skybox/black.png", "images/skybox/black.png" };

	ResourceManager::defaultTexture = ResourceManager::LoadTexture("images/T_DefaultTexture.png", Texture::Type::albedo);
	ResourceManager::defaultNormal = ResourceManager::LoadTexture("images/T_Normal.png", Texture::Type::normal);
	ResourceManager::defaultMaterial = ResourceManager::LoadMaterial("Default Material", ResourceManager::super);
	ResourceManager::defaultMaterial->AddTextures({
		ResourceManager::defaultTexture,
		ResourceManager::defaultNormal
		});
	ResourceManager::defaultShader = ResourceManager::super;

	UserPreferences::Initialise();
	PrefabManager::Initialise();

	// TODO: Use a shader asset
	Shader* particleShader = ResourceManager::LoadShader("particle", Shader::Flags::VPmatrix);
	scene->audio.Initialise();

	scene->Start();

	scene->renderSystem.Start(
	);

	SwitchToWindowMode((WindowModes)UserPreferences::windowedStartMode);



	for (auto i = scene->transforms.begin(); i != scene->transforms.end(); i++)
	{
		(*i).second.UpdateGlobalMatrixCascading();
	}


	std::cout << "Start finished\n";
}

SceneManager::~SceneManager()
{
	// TODO: Call unload scene or something in place of destructor
	//delete scene;
	
	scene->audio.Deinitialise();

	// Textures
	ResourceManager::UnloadAll();


	//TODO: unload meshes properly

	// Close ImGui
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
	frameTimes.Push(deltaTime);
	deltaTime = fminf(maxFrameTime, deltaTime);

	ProcessInput(window);
	float orthoWidth = camera.getOrthoWidth();
	float orthoHeight = camera.getOrthoHeight();
	projection = camera.InOrthoMode() ?
		glm::ortho(-orthoWidth/2.0f, orthoWidth / 2.0f, -orthoHeight / 2.0f, orthoHeight / 2.0f, camera.nearPlane, camera.farPlane) :
		glm::perspective(glm::radians(camera.fov), (float)windowWidth / (float)windowHeight, camera.nearPlane, camera.farPlane);
	view = camera.GetViewMatrix();
	viewProjection = projection * view;

	// Delete sceneobjects marked for deletion
	scene->DeleteSceneObjectsMarkedForDelete();

	//// TODO: Actual draw/update loop
	scene->renderSystem.projection = projection;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGuizmo::BeginFrame();
	

	if (scene->gui.getSelected()) {
		scene->gui.getSelected()->DebugDraw();
	}
	else if (!scene->gui.getMultiSelected().empty()) {
		auto& s = scene->gui.getMultiSelected();
		for (auto i : s)
		{
			i->DebugDraw();
		}
	}


	scene->renderSystem.lines.Compile();
	scene->renderSystem.debugLines.Compile();


	for (auto& s : ResourceManager::shaders)
	{
		if (s.second.getFlag() & Shader::Flags::VPmatrix) {
			s.second.Use();
			s.second.setMat4("vp", viewProjection);
		}
	}

	scene->Draw(deltaTime);
	scene->renderSystem.lines.Clear();
	scene->renderSystem.debugLines.Clear();
	scene->gui.Update();
	ImGui::Render();
	scene->Update(deltaTime);

	// TODO: remove if if not needed
	//if (ImGui::GetDrawData())
	//{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//}

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(window);
	}

	// Check and call events and swap the buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool SceneManager::ShouldClose() const
{
	return glfwWindowShouldClose(window);
}

void SceneManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	if (width == 0 && height == 0) {
		width = height = 1;
	}
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;

	scene->renderSystem.ScreenResize(windowWidth, windowHeight);
	scene->OnWindowResize();
}

void SceneManager::MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (lockedCamera) { 
		cursorPos = { xposIn / windowWidth, 1 - yposIn / windowHeight };
	}
	else {
		glfwSetCursorPos(window, 0, 0);

		if (firstMouse)
		{
			firstMouse = false;
			xposIn = 0.0;
			yposIn = 0.0;
		}
	}


	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos;
	float yoffset = -ypos;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void SceneManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse) { return; }
	camera.ProcessMouseScroll((float)yoffset);
}

void GLAPIENTRY SceneManager::ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// TODO: Find if there are definitions or values rather than having the int IDs by themselves
	// Ignore errors we don't care about (NVidia drivers)
	// #131169 - Framebuffer detailed info
	// #131185 - Buffer detailed info
	// #131218 - Program/shader state performance warning
	// #131204 - Texture state usage warning
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

	std::cout << "---------------\n";
	std::cout << "Debug message (" << id << "): " << message << "\n";

	std::cout << "Source: ";
	switch (source)
	{
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Window System";   break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Third Party";	  break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Application";	  break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Other";			  break;
	case GL_DEBUG_SOURCE_API:             std::cout << "API";			  break;
	}
	std::cout << "\n";

	std::cout << "Type: ";
	switch (type)
	{
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Undefined Behaviour";  break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Portability";			 break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Performance";			 break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Push Group";			 break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Pop Group";			 break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Marker";				 break;
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Error";				 break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Other";				 break;
	}
	std::cout << "\n";

	std::cout << "Severity: ";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "high";		  break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "medium";		  break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "low";		  break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "notification"; break;
	}
	std::cout << "\n";
}

void SceneManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureKeyboard) { return; }

	if ((key == GLFW_KEY_F11 && action == GLFW_PRESS) || (key == GLFW_KEY_ENTER && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)) {
		ToggleFullscreen();
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		scene->Save();
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		scene->Load();
	}

	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		scene->gui.moveSelection--;
	}
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		scene->gui.moveSelection++;
	}

	scene->OnKeyDown(key, action);
}

void SceneManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse) { return; }

	//std::cout << "mouse callbacked!\n";

	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		scene->OnMouseDown();
	}
}

void SceneManager::ProcessInput(GLFWwindow* window)
{
	ProcessKeyboardInput(window);
	ProcessMouseInput(window);
}

void SceneManager::ProcessKeyboardInput(GLFWwindow* window)
{
	if (ImGui::GetIO().WantCaptureKeyboard) { return; }

	if (UserPreferences::escapeCloses && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Camera movement
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::DOWN, deltaTime); }
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::FORWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::BACKWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::LEFT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::RIGHT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::UP, deltaTime); }

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (scene->gui.getSelected()) {
			camera.transform.LookAt(scene->gui.getSelected()->transform()->getGlobalPosition(), {0.0f, 1.0f, 0.0f});
		}
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) != camera.artKeyDown) {
		camera.artKeyDown = !camera.artKeyDown;
		if (camera.state == Camera::State::artEditorMode) {
			lockedCamera = !camera.artKeyDown;
			RefreshInputMode();
		}
	}

	for (auto i = scene->inputKeyWatch.begin(); i != scene->inputKeyWatch.end(); i++)
	{
		scene->OnKey(*i, glfwGetKey(window, *i));
	}
}

void SceneManager::ProcessMouseInput(GLFWwindow* window)
{
	if (ImGui::GetIO().WantCaptureMouse) { return; }

	// TODO: Some of this should probably be handled within the camera
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) { camera.artState = Camera::ArtState::orbit; }
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) { camera.artState = Camera::ArtState::dolly; }
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) { camera.artState = Camera::ArtState::boomTruck; }
	else { camera.artState = Camera::ArtState::none; }

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != (int)camera.editorRotate) {
		camera.editorRotate = !camera.editorRotate;
		if (camera.state == Camera::State::editorMode) {
			lockedCamera = !camera.editorRotate;
			RefreshInputMode();
		}
	}
}

void SceneManager::ToggleFullscreen()
{
	switch (windowMode)
	{
	case WindowModes::windowed:
		SwitchToWindowMode(WindowModes::maximised);
		break;
	case WindowModes::maximised:
		SwitchToWindowMode(WindowModes::borderlessFullscreen);
		break;
	case WindowModes::borderlessFullscreen:
		SwitchToWindowMode(WindowModes::windowed);
		break;
	default:
		break;
	}
}

void SceneManager::SwitchToWindowMode(WindowModes mode)
{
	windowMode = mode;
	auto temp = glfwGetVideoMode(glfwGetPrimaryMonitor());
	firstMouse = true;
	switch (mode)
	{
	case WindowModes::windowed:
		glfwRestoreWindow(window);
		glfwSetWindowMonitor(window, nullptr, windowWidth / 4, windowHeight / 4, windowWidth / 2, windowHeight / 2, temp->refreshRate);
		break;
	case WindowModes::borderlessFullscreen:
		glfwRestoreWindow(window);
		glfwSetWindowMonitor(SceneManager::window, glfwGetPrimaryMonitor(), 0, 0, temp->width, temp->height, temp->refreshRate);
		break;
	case WindowModes::maximised:
		glfwMaximizeWindow(window);
		break;
	default:
		break;
	}
}

void SceneManager::RefreshInputMode()
{
	if (lockedCamera) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window, windowWidth / 2.f, windowHeight / 2.f);
		firstMouse = true;
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}
