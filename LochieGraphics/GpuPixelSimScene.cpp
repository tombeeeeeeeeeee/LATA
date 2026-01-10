#include "GpuPixelSimScene.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"
#include "EditorGUI.h"

#include "Paths.h"

void GpuPixelSimScene::LoadShaders()
{
	pixelShader = ResourceManager::LoadShader("simplePixelGpu");
	//simple2dShader = ResourceManager::LoadShader("ui");
	simple2dShader = ResourceManager::LoadShader("2dShader");
	pixelSim.LoadComputeShaders();
}

void GpuPixelSimScene::Start()
{
	pixelSim.Initialise();

	texture = ResourceManager::CreateTexture(pixelSim.chunkWidth, pixelSim.chunkHeight, GL_RGBA, nullptr, GL_CLAMP_TO_BORDER, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(pixelSim.chunkWidth, pixelSim.chunkHeight, texture, nullptr, false);

	LoadShaders();
	//displayGUI = false;

	quad.InitialiseQuad(1.0f);

	camera->transform.setEulerRotation({ 0.0f, 180.0f, 0.0f });
	camera->transform.setPosition({ pixelSim.chunkWidth / 2.0f, pixelSim.chunkHeight / 2.0f, camera->transform.getPosition().z });
	camera->editorOrth = true;
	camera->orthoScale = 600;
	camera->state = Camera::State::tilePlacing;
}

glm::vec2 GpuPixelSimScene::ScreenToWorld(glm::vec2 screenPos)
{
	float ratio = (float)*windowWidth / (float)*windowHeight;

	// Centre screen pos, X and Y should be between -0.5 to 0.5
	glm::vec2 pos = screenPos - glm::vec2(0.5f, 0.5f);
	// Adjust scale
	pos.x *= ratio;
	pos *= camera->orthoScale;

	float camX = camera->transform.getPosition().x;
	float camY = camera->transform.getPosition().y;

	float camSin = sinf(glm::radians(glm::degrees(glm::eulerAngles(camera->transform.getRotation())).z));
	float camCos = cosf(glm::radians(glm::degrees(glm::eulerAngles(camera->transform.getRotation())).z));

	return glm::vec2(
		camX - (pos.x * camCos - pos.y * camSin),
		camY - (pos.y * camCos + pos.x * camSin)
	);
}

void GpuPixelSimScene::Update(float delta)
{
	if (update || updateOnce)
	{
		updateOnce = false;
		pixelSim.Update(delta);
	}

	if (glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		PixelsGPU::CellPixel cell;
		cell.matID = placingMatID;
		
		glm::ivec2 worldCurrentCursorPos = ScreenToWorld(*cursorPos);
		glm::ivec2 worldPreviousCursorPos = ScreenToWorld(previousCursorPos);

		glm::vec2 vel = *cursorPos - previousCursorPos;

		static float velMultiplier = 16.0f;

		vel *= velMultiplier;
		cell.vel = vel;
		
		pixelSim.SetCircleTo(worldCurrentCursorPos, placingRadius, cell);
	}

	previousCursorPos = *cursorPos;
}


void GpuPixelSimScene::Draw(float delta)
{
	renderSystem.Update(transforms, pointLights, spotlights, camera, delta, nullptr, &pixelSim, frameBuffer, pixelShader, quad, simple2dShader, texture, renderIndex);
}

void GpuPixelSimScene::GUI()
{
	ImGui::Checkbox("Update", &update);
	if (ImGui::Button("Update once"))
	{
		updateOnce = true;
	}
	if (ImGui::Button("Reload shaders"))
	{
		LoadShaders();
	}
	ImGui::InputInt("MatID placing", &placingMatID);
	ImGui::SliderFloat("Placing radius", &placingRadius, 0.0f, (pixelSim.chunkWidth + pixelSim.chunkHeight) / 2);
	ImGui::InputInt("RenderIndex", &renderIndex);

	pixelSim.GUI();
}
