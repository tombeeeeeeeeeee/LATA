#include "TestScene.h"

#include "Camera.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"

#include "Utilities.h"

#include "Paths.h"

#include "EditorGUI.h"


TestScene::TestScene()
{
}

void TestScene::Start()
{
	// PixelStuff setup
	pixelStuff.SetSimpleMaterials();
	pixelStuff.SetDebugColours();

	// Rendering preperations
	texture = ResourceManager::CreateTexture(PIXELS_W, PIXELS_H, GL_SRGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);
	frameBuffer = new FrameBuffer(PIXELS_W, PIXELS_H, texture, nullptr, false);

	pixelShader = ResourceManager::LoadShader("simplePixel");
	pixelShader->Use();
	pixelShader->setInt("gridCols", PIXELS_W);
	pixelShader->setInt("gridRows", PIXELS_H);

	overlayShader = ResourceManager::LoadShader("Shaders/default.vert", "Shaders/simpleTexturedWithCutout.frag");

	simple2dShader = ResourceManager::LoadShader("2dShader");

	// A quad
	quad.InitialiseQuad(1.0f);

	// Set up pixel colour SSBO, this is how the pixel colours are stored on the GPU
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// TODO: This shouldn't be a "PixelData" or something, as we wouldn't need to upload everything to the GPU (Probably just the colour)
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PixelStuff::GpuPixelData) * PIXELS_W * PIXELS_H, pixelStuff.GetGpuPixelToDrawFrom(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	camera->transform.setEulerRotation({ 0.0f, 180.0f, 0.0f });
	camera->editorOrth = true;
}

void TestScene::Update(float delta)
{
	if (updateSim) {
		pixelStuff.UpdateSim();
	}

	// Convert mouse cursor position to pixelspace
	guiCursor = glm::ivec2{ cursorPos->x * PIXELS_W, cursorPos->y * PIXELS_H };
	// Ensure imgui isn't using mouse
	if (!ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(renderSystem.window, GLFW_MOUSE_BUTTON_LEFT)) {
		pixelStuff.SetCircleToMaterial(guiCursor.x, guiCursor.y, selectEditRadius, selectMat);
	}
}

void TestScene::Draw(float delta)
{
	// Update the gpu version in preperation to send to GPU
	pixelStuff.PrepareDraw();
	

	// Note: Not using the render system for anything yet, haven't changed anything with it yet, should be fine either commented or not
	//renderSystem.Update(
	//	transforms,
	//	pointLights,
	//	spotlights,
	//	camera,
	//	delta
	//);


	// Update SSBO, the pixel colour data
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// TODO: This shouldn't be a "PixelData" or something, as we wouldn't need to upload everything to the GPU (Probably just the colour)
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(PixelStuff::GpuPixelData) * PIXELS_W * PIXELS_H, pixelStuff.GetGpuPixelToDrawFrom());

	// Note: The framebuffer is not required for the current set up, is here for the sake of it at the moment
	// Bind framebuffer for drawing pixels
	frameBuffer->Bind();
	glViewport(0, 0, PIXELS_W, PIXELS_H);
	
	// Draw pixels
	pixelShader->Use();
	quad.Draw();


	// Unbind framebuffer
	FrameBuffer::Unbind();
	// TODO: Is there a rendersystem function for this, if not maybe there should be something similar
	glViewport(0, 0, renderSystem.SCREEN_WIDTH / renderSystem.superSampling, renderSystem.SCREEN_HEIGHT / renderSystem.superSampling);


	//// Draw framebuffer texture to screen
	//overlayShader->Use();
	//overlayShader->setFloat("material.alpha", 1.0f);

	//texture->Bind(1);
	//overlayShader->setSampler("material.albedo", 1);

	//quad.Draw();

	simple2dShader->Use();
	// TODO: Forgot when viewMatrix gets updated, should be using that or just get from the camera, check
	// renderSystem.viewMatrix
	// Remove camera include if not needed
	simple2dShader->setMat4("vp", SceneManager::viewProjection);
	simple2dShader->setMat4("model", glm::mat4(1000.0f));
	texture->Bind(1);
	simple2dShader->setSampler("albedo", 1);


	quad.Draw();
}

void TestScene::GUI()
{
	if (!ImGui::Begin("Test Scene!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
	}

	ImGui::Checkbox("Update Sim", &updateSim);
	ImGui::SameLine();
	if (ImGui::Button("Update Frame")) {
		pixelStuff.UpdateSim();
	}

	ImGui::BeginDisabled();
	
	int amountOfAir = pixelStuff.AmountOf(0);
	ImGui::InputInt("Amount of Air", &amountOfAir);

	ImGui::EndDisabled();

	ImGui::ColorEdit3("Colour to set", &pickerColour.x);

	ImGui::DragFloat("Select Edit Radius", &selectEditRadius);

	ImGui::InputScalar("Material", ImGuiDataType_U32, &selectMat);

	glm::ivec2 cursorPixelPos = { Utilities::PositiveMod(guiCursor.x, PIXELS_W), Utilities::PositiveMod(guiCursor.y, PIXELS_H) };


	if (ImGui::Button("Set everything to above material")) {
		pixelStuff.SetEverythingTo(selectMat);
	}


	if (ImGui::Button("Set select to above material")) {
		pixelStuff.SetCircleToMaterial(guiCursor.x, guiCursor.y, selectEditRadius, selectMat);
	}


	if (ImGui::Button("Set everything to colour")) {
		pixelStuff.SetEverythingToColour(pickerColour);
	}

	if (ImGui::Button("Set everything to default material colours")) {
		pixelStuff.SetAllToDefaultColour();
	}

	ImGui::DragInt2("Gui Cursor", &guiCursor.x);

	if (ImGui::CollapsingHeader("Selected Info")) {
		ImGui::SeparatorText("Pixel Info");
		pixelStuff.PixelGUI(cursorPixelPos.x, cursorPixelPos.y);
	}

	ImGui::End();
}

TestScene::~TestScene()
{
}
