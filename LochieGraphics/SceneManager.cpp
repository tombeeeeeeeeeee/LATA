#include "SceneManager.h"

#include "Scene.h"
#include "Skybox.h"
#include "ResourceManager.h"
#include "ShaderEnum.h"
#include "SceneObject.h"
#include "Lights.h"

// This includes imgui differently then other files as it is managed here
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "ImGuiStyles.h"

#include <iostream>
#include <array>

unsigned int SceneManager::windowWidth = 1920 / 2;
unsigned int SceneManager::windowHeight = 1080 / 2;
WindowModes SceneManager::windowMode = WindowModes::windowed;
GLFWwindow* SceneManager::window = nullptr;
Scene* SceneManager::scene = nullptr;
glm::vec2 SceneManager::cursorPos = { 0, 0 };

float SceneManager::deltaTime = 0.0f;
FixedSizeQueue<float, 300> SceneManager::frameTimes;

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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//TODO:
	//glfwWindowHint(GLFW_SAMPLES, 16); // for MSAA

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

	RefreshInputMode();
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
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

	// ImGui style
	ImGuiStyles::SetLocStyle();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(); // TODO: Theres an overload for this that takes a version, see if should be using


	// Depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Face culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//TODO:
	// MSAA
	//glEnable(GL_MULTISAMPLE);

	// Draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	scene->renderSystem.window = window;

	camera.nearPlane = 0.5f;
	camera.farPlane = 100.0f;
	scene->camera = &camera;

	scene->windowWidth = &windowWidth;
	scene->windowHeight = &windowHeight;

	scene->cursorPos = &cursorPos;

	scene->shaders.insert(scene->shaders.end(), {
		ResourceManager::LoadShader("shaders/cubemap.vert", "shaders/cubemap.frag"),
		ResourceManager::LoadShader("shaders/simpleDepthShader.vert", "shaders/simpleDepthShader.frag"),
		ResourceManager::LoadShader("shaders/shadowDebug.vert", "shaders/shadowDebug.frag"),
		ResourceManager::LoadShaderDefaultVert("HDRBloom"),
		ResourceManager::LoadShaderDefaultVert("brdf"),
		ResourceManager::LoadShader("prefilter"),
		ResourceManager::LoadShaderDefaultVert("downSample"),
		ResourceManager::LoadShaderDefaultVert("upSample"),
		ResourceManager::LoadShader("irradiance"),
		ResourceManager::LoadShader("lineRenderer", Shader::Flags::VPmatrix),
		ResourceManager::LoadShaderDefaultVert("ssao"),
		ResourceManager::LoadShaderDefaultVert("ssaoBlur"),
		ResourceManager::LoadShader("forward", Shader::Flags::VPmatrix),
		ResourceManager::LoadShader("shaders/superDuper.vert", "shaders/superDuper.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix | Shader::Flags::Spec),
	});

	std::array<std::string, 6> skyboxFaces = { "images/SkyBox Volume 2/Stars01/leftImage.png", "images/SkyBox Volume 2/Stars01/rightImage.png", "images/SkyBox Volume 2/Stars01/upImage.png", "images/SkyBox Volume 2/Stars01/downImage.png", "images/SkyBox Volume 2/Stars01/frontImage.png", "images/SkyBox Volume 2/Stars01/backImage.png" };
	defaultSkybox = new Skybox(scene->shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data()));

	if (scene->skybox == nullptr) {
		scene->skybox = defaultSkybox;
	}

	ResourceManager::defaultTexture = ResourceManager::LoadTexture("images/T_DefaultTexture.png", Texture::Type::albedo);
	ResourceManager::defaultMaterial = ResourceManager::LoadMaterial("Default Material", scene->shaders[super]);
	ResourceManager::defaultMaterial->AddTextures({
		ResourceManager::defaultTexture
		});
	ResourceManager::defaultShader = scene->shaders[super];

	scene->Start();

	scene->renderSystem.Start(
		scene->skybox->texture,
		&scene->shaders,
		scene->lights.front(),
		""
	);



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

	ProcessInput(window);
	float orthoWidth = camera.getOrthoWidth();
	float orthoHeight = camera.getOrthoHeight();
	projection = camera.InOrthoMode() ?
		glm::ortho(-orthoWidth/2.0f, orthoWidth / 2.0f, -orthoHeight / 2.0f, orthoHeight / 2.0f, camera.nearPlane, camera.farPlane) :
		glm::perspective(glm::radians(camera.fov), (float)windowWidth / (float)windowHeight, camera.nearPlane, camera.farPlane);
	view = camera.GetViewMatrix();
	viewProjection = projection * view;

	// Delete sceneobjects marked for deletion
	while (!scene->markedForDeletion.empty())
	{
		if (scene->gui.sceneObjectSelected) {
			if (scene->gui.sceneObjectSelected->GUID == scene->markedForDeletion.front()) {
				scene->gui.sceneObjectSelected = nullptr;
			}
		}
		delete scene->sceneObjects.at(scene->markedForDeletion.front());
		scene->sceneObjects.erase(scene->markedForDeletion.front());
		scene->markedForDeletion.erase(scene->markedForDeletion.begin());
	}

	//// TODO: Actual draw/update loop
	scene->renderSystem.projection = projection;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGuizmo::BeginFrame();
	
	scene->renderSystem.lines.Clear();
	scene->Update(deltaTime);
	scene->renderSystem.lines.Compile();

	scene->skybox->Update(&camera, (float)windowWidth / (float)windowHeight);

	for (auto s = scene->shaders.begin(); s != scene->shaders.end(); s++)
	{
		if (((*s)->getFlag() & Shader::Flags::Lit)) {
			for (auto l = scene->lights.begin(); l != scene->lights.end(); l++)
			{
				(*l)->ApplyToShader(*s);
				(*s)->setVec3("viewPos", camera.transform.getPosition());
			}
		}
		if ((*s)->getFlag() & Shader::Flags::VPmatrix) {
			(*s)->Use();
			(*s)->setMat4("vp", viewProjection);
		}
	}
	scene->Draw();
	scene->gui.Update();
	ImGui::Render();

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

	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
		scene->Save();
	}
	if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
		scene->Load();
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

	// TODO: Remove this, maybe make a modal pop up window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Camera movement
	float cameraSpeed = 2.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::FORWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::BACKWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::LEFT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::RIGHT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::UP, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::DOWN, deltaTime); }

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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != camera.editorRotate) {
		camera.editorRotate = !camera.editorRotate;
		if (camera.state == Camera::State::editorMode) {
			lockedCamera = !camera.editorRotate;
			RefreshInputMode();
		}
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
		glfwSetWindowMonitor(window, nullptr, windowWidth / 4, windowHeight / 4, windowWidth / 2, windowHeight / 2, temp->refreshRate);
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
