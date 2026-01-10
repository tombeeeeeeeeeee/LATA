#include "GpuPixels.h"

#include "Graphics.h"
#include "Paths.h"
#include "EditorGUI.h"

#include <functional>

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

	int maxComputeSSBOs;
	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &maxComputeSSBOs);

	updatePixels = new ComputeShader(Paths::importShaderLocation + "update" + Paths::computeExtension);
	preUpdate = new ComputeShader(Paths::importShaderLocation + "preUpdate" + Paths::computeExtension);
	placeCircle = new ComputeShader(Paths::importShaderLocation + "drawCircle" + Paths::computeExtension);
	testCompute = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);
	testCompute2 = new ComputeShader(Paths::importShaderLocation + "testPixelCompute2" + Paths::computeExtension);
}

void PixelsGPU::Simulation::Chunk::BindCorrectSSBO() const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo1);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, writeSsbo1 ? ssbo1 : ssbo2);
}

void PixelsGPU::Simulation::Chunk::SwitchReadWriteSSBOs()
{
	readSsboFirst = !readSsboFirst;
}

PixelsGPU::Simulation::Chunk::Chunk(int _x, int _y) :
	x(_x),
	y(_y)
{
	glGenBuffers(1, &ssbo1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);

	glBufferData(GL_SHADER_STORAGE_BUFFER, CalculateSsboSize(), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo1);
	readSsboFirst = true;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

PixelsGPU::Simulation::Chunk::~Chunk()
{
	// TODO: how to clear free ssbo
}

void PixelsGPU::Simulation::Initialise()
{
	constexpr int r = 1;
	for (int x = -r; x < r+1; x++)
	{
		for (int y = -r; y < r+1; y++)
		{
			chunks.emplace_back(x, y);
		}
	}
}

void PixelsGPU::Simulation::Update(float delta)
{
	timer += delta;
	for (int i = 0; i < subUpdates; i++)
	{
		preUpdate->Use();
		preUpdate->setInt("gridCols", Simulation::chunkWidth);
		preUpdate->setInt("gridRows", Simulation::chunkHeight);
		preUpdate->setInt("updateCount", updateCount);
		for (auto& chunk : chunks)
		{
			chunk.BindCorrectSSBO();
			preUpdate->setBool("readSsboFirst", chunk.readSsboFirst);
			preUpdate->setIVec2("chunkCoords", glm::ivec2(chunk.x, chunk.y));
			preUpdate->Run(Simulation::chunkWidth / computeLocalSizeX, Simulation::chunkHeight / computeLocalSizeY, computeLocalSizeZ, GL_SHADER_STORAGE_BARRIER_BIT);
			chunk.SwitchReadWriteSSBOs();
		}

		updatePixels->Use();
		updatePixels->setInt("gridCols", Simulation::chunkWidth);
		updatePixels->setInt("gridRows", Simulation::chunkHeight);
		updatePixels->setInt("updateCount", updateCount);
		for (auto& chunk : chunks)
		{
			chunk.BindCorrectSSBO();
			updatePixels->setBool("readSsboFirst", chunk.readSsboFirst);
			updatePixels->setIVec2("chunkCoords", glm::ivec2(chunk.x, chunk.y));
			updatePixels->Run(Simulation::chunkWidth / computeLocalSizeX, Simulation::chunkHeight / computeLocalSizeY, computeLocalSizeZ, GL_SHADER_STORAGE_BARRIER_BIT);
			chunk.SwitchReadWriteSSBOs();
		}
		++updateCount;
	}
}

size_t PixelsGPU::Simulation::Chunk::CalculateSsboSize() const
{
	return sizeof(PixelsGPU::CellPixel) * Simulation::chunkWidth * Simulation::chunkHeight * 2;
}

void PixelsGPU::Simulation::SetCircleTo(glm::ivec2 pos, float radius, PixelsGPU::CellPixel cell)
{
	placeCircle->Use();
	placeCircle->setInt("gridCols", Simulation::chunkWidth);
	placeCircle->setInt("gridRows", Simulation::chunkHeight);

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

	for (auto& chunk : chunks)
	{
		chunk.BindCorrectSSBO();
		placeCircle->setBool("readSsboFirst", chunk.readSsboFirst);
		placeCircle->setIVec2("chunkCoords", glm::ivec2(chunk.x, chunk.y));
		placeCircle->Run(Simulation::chunkWidth / computeLocalSizeX, Simulation::chunkHeight / computeLocalSizeY, computeLocalSizeZ, GL_SHADER_STORAGE_BARRIER_BIT);
		chunk.SwitchReadWriteSSBOs();
	}

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
