#include "GpuPixelSimScene.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"
#include "EditorGUI.h"

#include "Paths.h"

void GpuPixelSimScene::LoadShaders()
{
	pixelShader = ResourceManager::LoadShader("simplePixelGpu");
	simple2dShader = ResourceManager::LoadShader("ui");
	if (updatePixels)
	{
		updatePixels->DeleteProgram();
	}
	updatePixels = new ComputeShader(Paths::importShaderLocation + "update" + Paths::computeExtension);
	placeCircle = new ComputeShader(Paths::importShaderLocation + "drawCircle" + Paths::computeExtension);
	testCompute = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);
}

void GpuPixelSimScene::BindCorrectReadWriteSSBOs()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, *readSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, *writeSsbo);
}

void GpuPixelSimScene::SwitchReadWriteSSBOs()
{
	readSsbo = readSsbo == &ssbo1 ? &ssbo2 : &ssbo1;
	writeSsbo = writeSsbo == &ssbo1 ? &ssbo2 : &ssbo1;
}

void GpuPixelSimScene::Start()
{
	glGenBuffers(1, &ssbo1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(CellPixel) * width * height, nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo1);
	readSsbo = &ssbo1;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &ssbo2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(CellPixel) * width * height, nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo2);
	writeSsbo = &ssbo2;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	texture = ResourceManager::CreateTexture(width, height, GL_RGBA, nullptr, GL_CLAMP_TO_BORDER, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(width, height, texture, nullptr, false);

	LoadShaders();
	//displayGUI = false;

	quad.InitialiseQuad(1.0f);
}

void GpuPixelSimScene::Update(float delta)
{
	if (update || updateOnce)
	{
		updateOnce = false;
		BindCorrectReadWriteSSBOs();

		updatePixels->Use();
		updatePixels->setInt("gridCols", width);
		updatePixels->setInt("gridRows", height);

		updatePixels->Run(width, height, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
		SwitchReadWriteSSBOs();
	}

	if (glfwGetMouseButton(SceneManager::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		BindCorrectReadWriteSSBOs();

		placeCircle->Use();
		placeCircle->setInt("gridCols", width);
		placeCircle->setInt("gridRows", height);

		placeCircle->setIVec2("centerCoords", glm::ivec2(cursorPos->x * width, cursorPos->y * height));
		placeCircle->setFloat("radius", 5.0f);
		placeCircle->setInt("pixel.matID", placingMatID);
		
		placeCircle->Run(width, height, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
		SwitchReadWriteSSBOs();
	}
}


void GpuPixelSimScene::Draw(float delta)
{
	BindCorrectReadWriteSSBOs();


	frameBuffer->Bind();
	glViewport(0, 0, width, height);

	pixelShader->Use();
	pixelShader->setInt("gridCols", width);
	pixelShader->setInt("gridRows", height);
	quad.Draw();

	frameBuffer->Unbind();
	glViewport(0, 0, *windowWidth, *windowHeight);

	simple2dShader->Use();
	texture->Bind(1);
	simple2dShader->setSampler("material.albedo", 1);
	quad.Draw();
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
}
