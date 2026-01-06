#include "GpuPixels.h"

#include "Graphics.h"
#include "Paths.h"
#include "EditorGUI.h"

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
	for (int i = 0; i < subUpdates; i++)
	{
		timer += delta;

		BindCorrectReadWriteSSBOs();
		preUpdate->Use();
		preUpdate->setInt("gridCols", width);
		preUpdate->setInt("gridRows", height);
		preUpdate->setInt("frameCount", frameCount);
		preUpdate->Run(width / 32, height / 32, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
		SwitchReadWriteSSBOs();

		BindCorrectReadWriteSSBOs();
		updatePixels->Use();
		updatePixels->setInt("gridCols", width);
		updatePixels->setInt("gridRows", height);
		updatePixels->setInt("frameCount", frameCount);
		updatePixels->Run(width / 32, height / 32, 1u, GL_SHADER_STORAGE_BARRIER_BIT);
		SwitchReadWriteSSBOs();

		++frameCount;
	}
}

size_t PixelsGPU::Simulation::CalculateSsboSize()
{
	return sizeof(CellPixel) * width * height;
}

void PixelsGPU::Simulation::SetCircleTo(glm::ivec2 pos, float radius, PixelsGPU::CellPixel cell)
{
	BindCorrectReadWriteSSBOs();

	placeCircle->Use();
	placeCircle->setInt("gridCols", width);
	placeCircle->setInt("gridRows", height);

	placeCircle->setIVec2("centerCoords", pos);
	placeCircle->setFloat("radius", radius);
	placeCircle->setInt("pixel.matID", cell.matID);
	placeCircle->setVec2("pixel.vel", cell.vel);
	if (cell.matID == 2)
	{
		placeCircle->setVec4("pixel.colour", glm::vec4(sin(timer * PI) / 4.0f + 0.75f, 0.76f, sin(timer * PI * 4) / 6.0f + 0.17f, 1.0));
	}
	else if (cell.matID != 0)
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

void PixelsGPU::Simulation::SetCircleTo(int x, int y, float radius, PixelsGPU::CellPixel cell)
{
	SetCircleTo(glm::ivec2(x, y), radius, cell);
}

void PixelsGPU::Simulation::GUI()
{
	ImGui::Checkbox("debug test", &debugTest);
	ImGui::InputInt("Sub updates", &subUpdates);
}
