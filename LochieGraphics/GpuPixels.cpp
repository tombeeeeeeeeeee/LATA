#include "GpuPixels.h"

#include "Graphics.h"
#include "Paths.h"
#include "EditorGUI.h"
#include "Colour.h"

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

	if (maxComputeSSBOs < 10)
	{
		// We gonnna have an issue
		__debugbreak();
	}

	updatePixels = new ComputeShader(Paths::importShaderLocation + "update" + Paths::computeExtension);
	preUpdate = new ComputeShader(Paths::importShaderLocation + "preUpdate" + Paths::computeExtension);
	placeCircle = new ComputeShader(Paths::importShaderLocation + "drawCircle" + Paths::computeExtension);
	testCompute = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);
	testCompute2 = new ComputeShader(Paths::importShaderLocation + "testPixelCompute2" + Paths::computeExtension);
}

void PixelsGPU::Simulation::Chunk::BindSSBO(int index) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo1);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, writeSsbo1 ? ssbo1 : ssbo2);
}

void PixelsGPU::Simulation::Chunk::SwitchReadWriteSSBOs()
{
	readSsboFirst = !readSsboFirst;
}

void PixelsGPU::Simulation::Chunk::Initialise()
{
	glGenBuffers(1, &ssbo1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);

	glBufferData(GL_SHADER_STORAGE_BUFFER, CalculateSsboSize(), nullptr, GL_DYNAMIC_COPY);
	glClearNamedBufferData(ssbo1, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo1);
	readSsboFirst = true;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void PixelsGPU::Simulation::Chunk::Deinitalise()
{
	glDeleteBuffers(1, &ssbo1);
}

PixelsGPU::Simulation::Chunk::Chunk(glm::ivec2 _coords) :
	coords(_coords)
{
}

PixelsGPU::Simulation::Chunk::~Chunk()
{
}

void PixelsGPU::Simulation::Initialise()
{
	//constexpr int r = 1;
	//for (int x = -r; x < r + 1; x++)
	//{
	//	for (int y = -r; y < r + 1; y++)
	//	{
	//		chunks.emplace_back(glm::ivec2(x, y));
	//	}
	//}
	//constexpr int r = 4;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			CreateChunk(glm::ivec2(x, y));
		}
	}
	PixelsGPU::CellPixel cell;
	cell.matID = 1;
	cell.colour = 0xFFFFFFFF;
	SetCircleTo(0, 0, 999999999.0f, cell);
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
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		for (auto& chunk : chunks)
		{
			chunk.BindSSBO(chunk.centreIndexSSBO);
			preUpdate->setBool("readSsboFirst", chunk.readSsboFirst);
			preUpdate->setIVec2("chunkCoords", chunk.coords);
			preUpdate->Run(Simulation::chunkWidth / computeLocalSizeX, Simulation::chunkHeight / computeLocalSizeY, computeLocalSizeZ, 0);
			chunk.SwitchReadWriteSSBOs();
		}

		updatePixels->Use();
		updatePixels->setInt("gridCols", Simulation::chunkWidth);
		updatePixels->setInt("gridRows", Simulation::chunkHeight);
		updatePixels->setInt("updateCount", updateCount);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		for (auto& chunk : chunks)
		{
			chunk.BindSSBO(Chunk::centreIndexSSBO);

			int nearChunkStatus[std::size(Chunk::orderedNearIndexSSBO)];
			for (int nearI = 0; nearI < std::size(Chunk::orderedNearIndexSSBO); ++nearI)
			{
				const Chunk* nearChunk = getChunkAt(chunk.coords + Chunk::orderedNearChunksLocalOffsets[nearI]);
				if (nearChunk)
				{
					nearChunk->BindSSBO(Chunk::orderedNearIndexSSBO[nearI]);
				}
				nearChunkStatus[nearI] = (nearChunk) ? (nearChunk->readSsboFirst ? 1 : 2) : 0;
			}
			updatePixels->setIntArray("nearChunkStatus", nearChunkStatus, std::size(Chunk::orderedNearIndexSSBO));

			updatePixels->setBool("readSsboFirst", chunk.readSsboFirst);
			updatePixels->setIVec2("chunkCoords", chunk.coords);
			updatePixels->Run(Simulation::chunkWidth / computeLocalSizeX, Simulation::chunkHeight / computeLocalSizeY, computeLocalSizeZ, 0);
		}
		for (auto& chunk : chunks)
		{
			chunk.SwitchReadWriteSSBOs();
		}
		++updateCount;
	}
}

const PixelsGPU::Simulation::Chunk* PixelsGPU::Simulation::getChunkAt(glm::ivec2 chunkCoords)
{
	for (const auto& chunk : chunks)
	{
		if (chunk.coords == chunkCoords)
		{
			return &chunk;
		}
	}
	return nullptr;
}

PixelsGPU::Simulation::Chunk& PixelsGPU::Simulation::CreateChunk(glm::ivec2 chunkCoords)
{
	auto& chunk = chunks.emplace_back(chunkCoords);
	chunk.Initialise();
	return chunk;
}

void PixelsGPU::Simulation::DestroyChunk(glm::ivec2 chunkCoords)
{
	for (auto i = chunks.begin(); i != chunks.end(); ++i)
	{
		if (i->coords == chunkCoords)
		{
			i->Deinitalise();
			chunks.erase(i);
			return;
		}
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
		placeCircle->setUnsignedInt("pixel.colour", Colour::RGBAU32FromRGBA01(glm::vec4(sin(timer * PI) / 4.0f + 0.75f, 0.76f, sin(timer * PI * 4) / 6.0f + 0.17f, 1.0)));
	}
	else if (cell.matID != 0)
	{
		placeCircle->setUnsignedInt("pixel.colour", Colour::RGBAU32FromRGBA01(glm::vec4(0.9, 0.9, 0.9, 1.0f)));
	}
	else
	{
		placeCircle->setUnsignedInt("pixel.colour", Colour::RGBAU32FromRGBA01(glm::vec4(0, 0, 0, 1.0f)));
	}

	placeCircle->setFloat("timer", timer);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	for (auto& chunk : chunks)
	{
		chunk.BindSSBO(Chunk::centreIndexSSBO);
		placeCircle->setBool("readSsboFirst", chunk.readSsboFirst);
		placeCircle->setIVec2("chunkCoords", chunk.coords);
		placeCircle->Run(Simulation::chunkWidth / computeLocalSizeX, Simulation::chunkHeight / computeLocalSizeY, computeLocalSizeZ, 0);
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
