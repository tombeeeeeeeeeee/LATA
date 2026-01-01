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
	pixelSim.LoadComputeShaders();
}

void PixelsGPU::Simulation::LoadComputeShaders()
{
	if (updatePixels)
	{
		updatePixels->DeleteProgram();
	}
	updatePixels = new ComputeShader(Paths::importShaderLocation + "update" + Paths::computeExtension);
	placeCircle = new ComputeShader(Paths::importShaderLocation + "drawCircle" + Paths::computeExtension);
	testCompute = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);
}

void PixelsGPU::Simulation::BindCorrectReadWriteSSBOs()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, *readSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, *writeSsbo);
}

void PixelsGPU::Simulation::SwitchReadWriteSSBOs()
{
	readSsbo = readSsbo == &ssbo1 ? &ssbo2 : &ssbo1;
	writeSsbo = writeSsbo == &ssbo1 ? &ssbo2 : &ssbo1;
}

void PixelsGPU::Simulation::Initialise()
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
}

void PixelsGPU::Simulation::Update(float delta)
{
	BindCorrectReadWriteSSBOs();

	updatePixels->Use();
	updatePixels->setInt("gridCols", width);
	updatePixels->setInt("gridRows", height);

	updatePixels->Run(width, height, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
	SwitchReadWriteSSBOs();
}

void PixelsGPU::Simulation::SetCircleToMaterial(glm::ivec2 pos, float radius, int matID)
{
	BindCorrectReadWriteSSBOs();

	placeCircle->Use();
	placeCircle->setInt("gridCols", width);
	placeCircle->setInt("gridRows", height);

	placeCircle->setIVec2("centerCoords", pos);
	placeCircle->setFloat("radius", radius);
	placeCircle->setInt("pixel.matID", matID);

	placeCircle->Run(width, height, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
	SwitchReadWriteSSBOs();
}

void GpuPixelSimScene::Start()
{
	pixelSim.Initialise();

	texture = ResourceManager::CreateTexture(pixelSim.width, pixelSim.height, GL_RGBA, nullptr, GL_CLAMP_TO_BORDER, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(pixelSim.width, pixelSim.height, texture, nullptr, false);

	LoadShaders();
	//displayGUI = false;

	quad.InitialiseQuad(1.0f);
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
		pixelSim.SetCircleToMaterial(glm::ivec2(cursorPos->x * pixelSim.width, cursorPos->y * pixelSim.height), placingRadius, placingMatID);
	}
}


void GpuPixelSimScene::Draw(float delta)
{
	pixelSim.BindCorrectReadWriteSSBOs();

	frameBuffer->Bind();
	glViewport(0, 0, pixelSim.width, pixelSim.height);

	pixelShader->Use();
	pixelShader->setInt("gridCols", pixelSim.width);
	pixelShader->setInt("gridRows", pixelSim.height);
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
	ImGui::SliderFloat("Placing radius", &placingRadius, 0.0f, (pixelSim.width + pixelSim.height) / 2);
}
