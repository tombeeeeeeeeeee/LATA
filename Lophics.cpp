#include "Lophics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"




unsigned int Lophics::windowWidth = 800;
unsigned int Lophics::windowHeight = 600;

float Lophics::lastX = 400;
float Lophics::lastY = 300;
bool Lophics::firstMouse = true;

Camera Lophics::camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

float Lophics::deltaTime = 0.0f;

void Lophics::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

void Lophics::processInput(GLFWwindow* window)
{
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

void Lophics::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void Lophics::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);

}

void Lophics::
Start()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	window = glfwCreateWindow(windowWidth, windowHeight, "Lochie's Testing", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return;
		//TODO: fix
		//return -1;
	}	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Load OpenGl function pointers with glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return;
		//TODO: fix
		//return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// Create shaders
	lightingShader = Shader("shader.vert", "shader.frag");
	lightCubeShader = Shader("lightCube.vert", "lightCube.frag");

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// TODO: some texture manager to unload the textures, currently unloading manually at the end
	specularMap.Initialise("images/container2.png");
	diffuseMap.Initialise("images/container2_specular.png");
	boxMaterial = Material(&specularMap, &diffuseMap, &lightingShader);

	// shader configuration
	lightingShader.Use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setInt("material.emission", 2);
	lightingShader.setVec3("viewPos", camera.position);
	lightingShader.setFloat("material.shininess", 64.0f);

	// directional light
	lightingShader.setVec3("dirLight.direction", directionalLight.direction);
	lightingShader.setVec3("dirLight.ambient", directionalLight.ambient);
	lightingShader.setVec3("dirLight.diffuse", directionalLight.diffuse);
	lightingShader.setVec3("dirLight.specular", directionalLight.specular);

	// Point lights
	for (int i = 0; i < sizeof(pointLights) / sizeof(pointLights[0]); i++)
	{
		std::string index = std::to_string(i);
		lightingShader.setVec3("pointLights[" + index + "].position", pointLights[i].position);
		lightingShader.setVec3("pointLights[" + index + "].ambient", pointLights[i].ambient);
		lightingShader.setVec3("pointLights[" + index + "].diffuse", pointLights[i].diffuse);
		lightingShader.setVec3("pointLights[" + index + "].specular", pointLights[i].specular);
		lightingShader.setFloat("pointLights[" + index + "].constant", pointLights[i].constant);
		lightingShader.setFloat("pointLights[" + index + "].linear", pointLights[i].linear);
		lightingShader.setFloat("pointLights[" + index + "].quadratic", pointLights[i].quadratic);
	}

	//TODO:
	// spotLight
	lightingShader.setVec3("spotLight.position", camera.position);
	lightingShader.setVec3("spotLight.direction", camera.front);


	cubeMesh.InitialiseCube();
}

void Lophics::Update()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Input
	processInput(window);

	// Clear
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// light properties


	boxMaterial.Use();
	// material properties

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	lightingShader.setMat4("projection", projection);
	lightingShader.setMat4("view", view);

	// world transformation
	glm::mat4 model = glm::mat4(1.0f);
	lightingShader.setMat4("model", model);



	// render containers
	for (unsigned int i = 0; i < 10; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		lightingShader.setMat4("model", model);
		cubeMesh.Draw();
	}
		

	// also draw the lights themselves
	lightCubeShader.Use();
	lightCubeShader.setMat4("projection", projection);
	lightCubeShader.setMat4("view", view);

	// we now draw as many light bulbs as we have point lights.
	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLights[i].position);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		lightCubeShader.setMat4("model", model);
		cubeMesh.Draw();
	}

	// Check and call events and swap the buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Lophics::Stop()
{

	// De-allocate resources
	lightingShader.DeleteProgram();
	lightCubeShader.DeleteProgram();

	// Textures
	specularMap.Delete();
	diffuseMap.Delete();

	// Terminate and clear GLFW resources
	glfwTerminate();

	std::cout << "test\n";
}
