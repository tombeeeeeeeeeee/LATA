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
	camera->editorOrth = true;
	camera->state = Camera::State::tilePlacing;
}

static glm::ivec2 CursorToWorld(glm::vec2 cursor, int width, int height)
{
	return glm::ivec2(cursor.x * width, cursor.y * height);
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
		
		glm::ivec2 worldCurrentCursorPos = CursorToWorld(*cursorPos, pixelSim.chunkWidth, pixelSim.chunkHeight);
		glm::ivec2 worldPreviousCursorPos = CursorToWorld(previousCursorPos, pixelSim.chunkWidth, pixelSim.chunkHeight);

		glm::vec2 vel = *cursorPos - previousCursorPos;

		static float velMultiplier = 3.1f;

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
