#include "TestScene.h"

#include "Camera.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"

#include "Utilities.h"

#include "Paths.h"

#include "ExtraEditorGUI.h"

#include <iostream>

std::vector<glm::ivec2> TestScene::GeneratePathBetween(glm::ivec2 start, glm::ivec2 end)
{
	// Always contain the starting position
	std::vector<glm::ivec2> values = { start };
	if (start == end) {
		// Early return if the pixel locations are the same
		return values;
	}
	glm::vec2 checking = start;
	const float checkEvery = 0.5f;
	const glm::vec2 normal = glm::normalize(glm::vec2(end - start));
	const float checkDistance = glm::length(glm::vec2(end - start)) - 0.5f;
	const glm::vec2 offset = normal * checkEvery;
	bool last = false;
	const glm::vec2 startF = glm::vec2(start);
	for (glm::vec2 checking = startF + offset; !last; checking += offset)
	{
		if (glm::length(checking - startF) >= checkDistance) {
			last = true;
			checking = end;
		}
		glm::ivec2 checkingPixelPos = glm::ivec2(roundf(checking.x), roundf(checking.y));
		// Don't insert value if it is already in
		if (checkingPixelPos != values.back()) {
			values.push_back(checkingPixelPos);
		}
	}
	return values;
}

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
	camera->state = Camera::State::tilePlacing;

	pixelSim.testCentreGravity = true;
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
	guiCursor = glm::ivec2{ floorf(mouse.x * Pixels::chunkWidth), floorf(mouse.y * Pixels::chunkHeight) };
	if (!pixelSim.isCellAt(guiCursor.x, guiCursor.y)) {
		//guiCursor = previousGuiCursor;
	}
	// Ensure imgui isn't using mouse
	if (!ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(renderSystem.window, GLFW_MOUSE_BUTTON_LEFT)) {
		auto path = GeneratePathBetween(previousGuiCursor, guiCursor);
		switch (mouseMode)
		{
		case TestScene::MouseMode::None:
			break;
		case TestScene::MouseMode::Brush:
			glm::vec2 vel = (glm::vec2(guiCursor) - glm::vec2(previousGuiCursor)) / 10.0f;
			for (auto& i : path)
			{
				pixelSim.SetCircleToMaterial(i.x, i.y, selectEditRadius, selectMat);
				if (glfwGetKey(renderSystem.window, GLFW_KEY_X) != GLFW_PRESS) {
					pixelSim.AddVelocityToCircle(i.x, i.y, selectEditRadius, vel);
				}
			}
			break;
		case TestScene::MouseMode::Select:
			selectGuiCursor = guiCursor;
			break;
		default:
			break;
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
	if (ImGui::CollapsingHeader("Pixel Info")) {
		ExtraEditorGUI::ScopedIndent indent;
		if (mouseMode == MouseMode::Select) {
			pixelSim.PixelGUI(selectGuiCursor);
		}
		else {
			pixelSim.PixelGUI(guiCursor);
		}
	}
}

void TestScene::SelectedInfoGUI()
{
	ExtraEditorGUI::ScopedBegin open("Selected Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (!open)
	{
		return;
	}
	const Pixels::Simulation::Chunk* chunk = pixelSim.getChunk(guiCursor.x, guiCursor.y);
	if (chunk) {
		bool temp = chunk->prevUpdated;
		ImGui::Checkbox("Chunk updated!", &temp);
	}

	if (ImGui::CollapsingHeader("Pixel Info")) {
		ExtraEditorGUI::ScopedIndent indent;
		if (mouseMode == MouseMode::Select) {
			pixelSim.PixelGUI(selectGuiCursor);
		}
		else {
			pixelSim.PixelGUI(guiCursor);
		}
	}
}

void TestScene::StatsGUI()
{
	ExtraEditorGUI::ScopedBegin open("Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (!open)
	{
		return;
	}
	{
		ExtraEditorGUI::ScopedDisable disable;
		int chunkCount = pixelSim.getChunkCount();
		ImGui::DragInt("Chunk count", &chunkCount);

		ImGui::DragInt("Count of selected pixel mat", &lastCount);
		ImGui::SameLine();
	}
	if (ImGui::Button("Refresh / Update count")) {
		lastCount = pixelSim.getAmountOf(selectMat);
	}
	{
		ExtraEditorGUI::ScopedDisable disable;
		const auto& chunks = pixelSim.getChunks();
		int updatedCount = 0;
		for (auto& i : chunks)
		{
			if (i.prevUpdated) {
				updatedCount++;
			}
		}
		ImGui::DragInt("Chunk prev updated", &updatedCount);
	}
}

void TestScene::PixelMaterialsGUI()
{
	ExtraEditorGUI::ScopedBegin open("Pixel Materials", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (!open)
	{
		return;
	}
	for (size_t i = 0; i < pixelSim.materialInfos.size(); i++)
	{
		if (ImGui::CollapsingHeader(("Material ID: " + std::to_string(i) + " ").c_str()))
		{
			ExtraEditorGUI::ScopedIndent indent;
			pixelSim.materialInfos.at(i).GUI();
		}
	}
}

void TestScene::GUI()
{
	SelectedInfoGUI();
	StatsGUI();
	PixelMaterialsGUI();

	ExtraEditorGUI::ScopedBegin open("Test Scene!", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (!open) {
		return;
	}

	ImGui::Checkbox("Update Sim", &updateSim);
	ImGui::SameLine();
	if (ImGui::Button("Update Frame")) {
		pixelSim.Update();
	}

	if (ImGui::CollapsingHeader("Multithreaded Options")) {
		ExtraEditorGUI::ScopedIndent indent;
		ImGui::Checkbox("Multithreaded", &pixelSim.multithreaded);

		ImGui::BeginDisabled();

		ThreadPool& threadPool = pixelSim.getThreadPool();
		int temp = threadPool.getThreadCount();
		ImGui::DragInt("Thread count", &temp);
		ImGui::EndDisabled();
		if (ImGui::Button("Add thread!")) {
			threadPool.AddThread();
		}
	}

	if (ImGui::CollapsingHeader("PixelSim debug stuff"))
	{
		ExtraEditorGUI::ScopedIndent indent;
		if (ImGui::Button("Set debug colours")) {
			pixelSim.SetDebugColours();
		}

		if (ImGui::Button("Toggle Draw Velocities")) {
			pixelSim.SetDrawVelocity(!pixelSim.getChunks()[0].drawVelocity);
		}

		ImGui::DragFloat2("Gravity", &pixelSim.gravityForce.x);
		ImGui::Checkbox("Centre Gravity", &pixelSim.testCentreGravity);

		ImGui::DragFloat("Radius", &pixelSim.radius);
		ImGui::DragFloat("Mass per cell", &pixelSim.massPerCell);
	}

	ImGui::ColorEdit3("Colour to set", &pickerColour.x);

	ImGui::DragFloat("Select Edit Radius", &selectEditRadius);

	ImGui::InputScalar("Material", ImGuiDataType_U32, &selectMat);

	if (ImGui::Button("Set everything to above material")) {
		pixelSim.SetEverythingTo(selectMat);
		pixelSim.SetAllToDefaultColour();
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

	ExtraEditorGUI::SliderEnum({ "None", "Brush", "Select", }, (int*)& mouseMode);

	ImGui::DragInt2("Gui Cursor", &guiCursor.x);
}

TestScene::~TestScene()
{
}
