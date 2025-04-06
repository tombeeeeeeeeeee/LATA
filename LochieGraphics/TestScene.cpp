#include "TestScene.h"

#include "Camera.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"

#include "Utilities.h"

#include "Paths.h"

#include "EditorGUI.h"

#include <iostream>

TestScene::TestScene()
{
}

void TestScene::Start()
{
	// pixelSim setup
	pixelSim.SetSimpleMaterials();
	pixelSim.SetCircleToMaterial( 1,  1, 0.5f, 3);
	pixelSim.SetCircleToMaterial( 1, -1, 0.5f, 3);
	pixelSim.SetCircleToMaterial(-1,  1, 0.5f, 3);
	pixelSim.SetCircleToMaterial(-1, -1, 0.5f, 3);
	//pixelSim.SetDebugColours();

	// Rendering preperations
	chunkTexture = ResourceManager::CreateTexture(Pixels::chunkWidth, Pixels::chunkHeight, GL_SRGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);
	chunkFrameBuffer = new FrameBuffer(Pixels::chunkWidth, Pixels::chunkHeight, chunkTexture, nullptr, false);

	pixelShader = ResourceManager::LoadShader("simplePixel");
	pixelShader->Use();
	pixelShader->setInt("gridCols", Pixels::chunkWidth);
	pixelShader->setInt("gridRows", Pixels::chunkHeight);

	overlayShader = ResourceManager::LoadShader("Shaders/default.vert", "Shaders/simpleTexturedWithCutout.frag");

	simple2dShader = ResourceManager::LoadShader("2dShader");

	// A quad
	quad.InitialiseQuad(1.0f);

	camera->transform.setEulerRotation({ 0.0f, 180.0f, 0.0f });
	camera->editorOrth = true;
}

void TestScene::Update(float delta)
{
	if (updateSim) {
		pixelSim.Update();
	}

	// Convert mouse cursor position to pixelspace
	// TODO: A function for ratio or something
	float ratio = (float)*windowWidth / (float)*windowHeight;
	glm::vec2 mouse = { 
		(cursorPos->x - 0.5f) * ratio * camera->orthoScale + camera->transform.getPosition().x,
		(cursorPos->y - 0.5f)* camera->orthoScale + camera->transform.getPosition().y };
	guiCursor = glm::ivec2{ mouse.x * Pixels::chunkWidth, mouse.y * Pixels::chunkHeight };
	if (!pixelSim.isCellAt(guiCursor.x, guiCursor.y)) {
		//guiCursor = previousGuiCursor;
	}
	// Ensure imgui isn't using mouse
	if (!ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(renderSystem.window, GLFW_MOUSE_BUTTON_LEFT)) {
		auto path = Pixels::GeneratePathBetween(previousGuiCursor, guiCursor);
		for (auto& i : path)
		{
			pixelSim.SetCircleToMaterial(i.x, i.y, selectEditRadius, selectMat);
			if (glfwGetKey(renderSystem.window, GLFW_KEY_X) != GLFW_PRESS) {
				glm::vec2 vel = (guiCursor - previousGuiCursor) / 10;
				pixelSim.AddVelocityToCircle(i.x, i.y, selectEditRadius, vel);
			}
		}
	}
	previousGuiCursor = guiCursor;
}

void TestScene::Draw(float delta)
{
	// Update the gpu version in preperation to send to GPU
	pixelSim.PrepareDraw(camera->transform.getPosition().x, camera->transform.getPosition().y);
	

	//// Note: Not using the render system for anything yet, haven't changed anything with it yet, should be fine either commented or not
	//renderSystem.Update(
	//	transforms,
	//	pointLights,
	//	spotlights,
	//	camera,
	//	delta
	//);
	
	for (auto& i : pixelSim.getChunks())
	{
		chunkFrameBuffer->Bind();
		glViewport(0, 0, Pixels::chunkWidth, Pixels::chunkHeight);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, i.ssbo);


		// Draw pixels
		pixelShader->Use();
		quad.Draw();

		FrameBuffer::Unbind();
		glViewport(0, 0, *windowWidth, *windowHeight);



		simple2dShader->Use();
		simple2dShader->setMat4("vp", SceneManager::viewProjection);
		glm::mat4 model = glm::mat4(0.5f);
		model = glm::translate(model, glm::vec3(i.x * 2.0f + 1.0f, i.y * 2.0f + 1.0f, 0.0f));
		simple2dShader->setMat4("model", model);
		chunkTexture->Bind(1);
		simple2dShader->setSampler("albedo", 1);
		quad.Draw();

		
		//break;
	}

	// Note: The framebuffer is not required for the current set up, is here for the sake of it at the moment
	// Bind framebuffer for drawing pixels
	


	// Unbind framebuffer
	// TODO: Is there a rendersystem function for this, if not maybe there should be something similar

;
	//// Draw framebuffer texture to screen
	//overlayShader->Use();
	//overlayShader->setFloat("material.alpha", 1.0f);

	//texture->Bind(1);
	//overlayShader->setSampler("material.albedo", 1);

	//quad.Draw();
}

void TestScene::GUI()
{
	if (!ImGui::Begin("Test Scene!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}

	ImGui::Checkbox("Update Sim", &updateSim);
	ImGui::SameLine();
	if (ImGui::Button("Update Frame")) {
		pixelSim.Update();
	}

	ImGui::BeginDisabled();
	
	int chunkCount = pixelSim.getChunkCount();
	ImGui::InputInt("Chunk count", &chunkCount);
	//int amountOfAir = pixelStuff.AmountOf(0);
	//ImGui::InputInt("Amount of Air", &amountOfAir);

	ImGui::EndDisabled();

	if (ImGui::Button("Set debug colours")) {
		pixelSim.SetDebugColours();
	}

	if (ImGui::Button("Toggle Draw Velocities")) {
		pixelSim.SetDrawVelocity(!pixelSim.getChunks()[0].drawVelocity);
	}

	ImGui::DragFloat2("Gravity", &pixelSim.gravityForce.x);
	ImGui::Checkbox("Centre Gravity", &pixelSim.testCenterGravity);

	ImGui::ColorEdit3("Colour to set", &pickerColour.x);

	ImGui::DragFloat("Select Edit Radius", &selectEditRadius);

	ImGui::InputScalar("Material", ImGuiDataType_U32, &selectMat);

	if (ImGui::Button("Set everything to above material")) {
		pixelSim.SetEverythingTo(selectMat);
	}


	if (ImGui::Button("Set select to above material")) {
		pixelSim.SetCircleToMaterial(guiCursor.x, guiCursor.y, selectEditRadius, selectMat);
	}


	if (ImGui::Button("Set everything to colour")) {
		pixelSim.SetEverythingToColour(pickerColour);
	}

	if (ImGui::Button("Set everything to default material colours")) {
		pixelSim.SetAllToDefaultColour();
	}

	ImGui::DragInt2("Gui Cursor", &guiCursor.x);

	if (ImGui::CollapsingHeader("Selected Info")) {
		ImGui::SeparatorText("Pixel Info");
		pixelSim.PixelGUI(guiCursor.x, guiCursor.y);
	}

	ImGui::End();
}

TestScene::~TestScene()
{
}

