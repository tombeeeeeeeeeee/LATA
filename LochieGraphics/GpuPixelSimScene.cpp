#include "GpuPixelSimScene.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"
#include "ExtraEditorGUI.h"
#include "SceneObject.h"

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
	pixelSim.InitialiseMaterials();
	LoadShaders();
	pixelSim.InitialiseChunks();

	texture = ResourceManager::CreateTexture(PixelsGPU::chunkWidth, PixelsGPU::chunkHeight, GL_RGBA, nullptr, GL_CLAMP_TO_BORDER, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(PixelsGPU::chunkWidth, PixelsGPU::chunkHeight, texture, nullptr, false);

	//displayGUI = false;

	quad.InitialiseQuad(1.0f);

	camera->transform.setEulerRotation({ 0.0f, 180.0f, 0.0f });
	camera->transform.setPosition({ PixelsGPU::chunkWidth / 2.0f, PixelsGPU::chunkHeight / 2.0f, camera->transform.getPosition().z });
	camera->editorOrth = true;
	camera->orthoScale = 600;
	camera->state = Camera::State::tilePlacing;

	gui.showResourceMenu = true;

	update = UserPreferences::pixelsLaunchUpdateOn;
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

void GpuPixelSimScene::DrawCircle(glm::vec2 worldCurrentCursorPos)
{
	PixelsGPU::CellPixel cell;
	cell.matID = placingMatID;

	const glm::vec2 worldDifference = worldCurrentCursorPos - previousCursorPos;
	static constexpr float velMultiplier = 1.0f;
	const glm::vec2 vel = worldDifference * velMultiplier;
	cell.vel = vel;

	pixelSim.SetCircleTo(worldCurrentCursorPos, placingRadius, cell);

}

void GpuPixelSimScene::Update(float delta)
{
	if (update || updateOnce)
	{
		updateOnce = false;
		pixelSim.Update(delta);
	}

	glm::vec2 worldCurrentCursorPos = ScreenToWorld(*cursorPos);
	
	if (!ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		switch (mouseMode)
		{
		case GpuPixelSimScene::MouseMode::None:
			break;
		case GpuPixelSimScene::MouseMode::Brush:
			DrawCircle(worldCurrentCursorPos);
			break;
		case GpuPixelSimScene::MouseMode::SelectPixel:
			break;
		case GpuPixelSimScene::MouseMode::SelectChunk:
			glm::vec2 chunkSpacePos = worldCurrentCursorPos / glm::vec2(PixelsGPU::chunkWidth, PixelsGPU::chunkHeight);
			glm::ivec2 chunkCoord = glm::ivec2(floorf(chunkSpacePos.x), floorf(chunkSpacePos.y));
			chunkSelected = chunkCoord;
			break;
		case GpuPixelSimScene::MouseMode::COUNT:
			break;
		default:
			break;
		}
	}

	previousCursorPos = worldCurrentCursorPos;
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
	
	ExtraEditorGUI::SliderEnum("Mouse mode", { "None", "Brush", "Select Pixel", "Select Chunk" }, (int*)&mouseMode);
	ImGui::InputInt("MatID placing", &placingMatID);
	ImGui::SliderFloat("Placing radius", &placingRadius, 0.0f, (PixelsGPU::chunkWidth + PixelsGPU::chunkHeight) / 2);
	ImGui::InputInt("RenderIndex", &renderIndex);
	const PixelsGPU::Chunk* chunk = pixelSim.getChunkAt(chunkSelected);
	{
		ExtraEditorGUI::ScopedDisable disable;
		ImGui::DragInt2("Chunk Coords", &chunkSelected.x);
	}
	if (chunk)
	{
		if (ImGui::Button("Destroy Chunk"))
		{
			pixelSim.DestroyChunk(chunkSelected);
		}
		ExtraEditorGUI::ScopedDisable disable;
		auto temp = chunk->ssbo1;
		ImGui::DragScalar("Chunk SSBO", ImGuiDataType_U32, &temp);
	}
	else
	{
		if (ImGui::Button("Create Chunk"))
		{
			pixelSim.CreateChunk(chunkSelected);
		}
	}

	pixelSim.GUI();
}
