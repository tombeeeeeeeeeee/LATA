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

	// Need to make sure these are all good limit wise when running on a worse device than mine
	int workGroupSize[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

	int workGroupCounts[3] = { 0 };
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

	int workGroupInvocations;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInvocations);

	updatePixels = new ComputeShader(Paths::importShaderLocation + "update" + Paths::computeExtension);
	preUpdate = new ComputeShader(Paths::importShaderLocation + "preUpdate" + Paths::computeExtension);
	placeCircle = new ComputeShader(Paths::importShaderLocation + "drawCircle" + Paths::computeExtension);
	testCompute = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);
	testCompute2 = new ComputeShader(Paths::importShaderLocation + "testPixelCompute2" + Paths::computeExtension);
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

	glBufferData(GL_SHADER_STORAGE_BUFFER, CalculateSsboSize(), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo1);
	readSsbo = &ssbo1;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &ssbo2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);

	glBufferData(GL_SHADER_STORAGE_BUFFER, CalculateSsboSize(), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo2);
	writeSsbo = &ssbo2;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void PixelsGPU::Simulation::Update(float delta)
{
	timer += delta;

	BindCorrectReadWriteSSBOs();

	preUpdate->Use();
	updatePixels->setInt("gridCols", width);
	updatePixels->setInt("gridRows", height);
	updatePixels->setInt("frameCount", frameCount);
	updatePixels->Run(width / 32, height / 32, 1u, GL_SHADER_STORAGE_BARRIER_BIT);

	// Instead of doing swaping the ssbos, could just have either one of have them swapped inside of the shader
	SwitchReadWriteSSBOs();
	BindCorrectReadWriteSSBOs();

	glBindBuffer(GL_COPY_READ_BUFFER, *readSsbo);
	glBindBuffer(GL_COPY_WRITE_BUFFER, *writeSsbo);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, CalculateSsboSize());
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // Lochie: Not sure on what should actually be here
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	updatePixels->Use();
	updatePixels->setInt("gridCols", width);
	updatePixels->setInt("gridRows", height);
	updatePixels->setInt("frameCount", frameCount);
	updatePixels->Run(width / 32, height / 32, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
	SwitchReadWriteSSBOs();

	

	++frameCount;
}

size_t PixelsGPU::Simulation::CalculateSsboSize()
{
	return sizeof(CellPixel) * width * height;
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
	if (matID == 2)
	{
		placeCircle->setVec4("pixel.colour", glm::vec4(sin(timer * PI) / 4.0f + 0.75f, 0.76f, sin(timer * PI * 4) / 6.0f + 0.17f, 1.0));
	}
	else if (matID != 0)
	{
		placeCircle->setVec4("pixel.colour", glm::vec4(0.9, 0.9, 0.9, 1.0f));
	}
	else
	{
		placeCircle->setVec4("pixel.colour", glm::vec4(0.0, 0.0, 0.0, 1.0f));
	}

	placeCircle->setFloat("timer", timer);

	placeCircle->Run(width / 32, height / 32, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
	SwitchReadWriteSSBOs();
}

void PixelsGPU::Simulation::SetCircleToMaterial(int x, int y, float radius, int matID)
{
	SetCircleToMaterial(glm::ivec2(x, y), radius, matID);
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
	pixelShader->setInt("renderIndex", renderIndex);
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
	ImGui::InputInt("RenderIndex", &renderIndex);
	ImGui::Checkbox("debug test", &pixelSim.debugTest);
}
