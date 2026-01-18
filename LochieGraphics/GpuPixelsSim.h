#pragma once

#include "ComputeShader.h"

#include "GpuPixelsCell.h"
#include "GpuPixelsChunk.h"
#include "GpuPixelsMat.h"

namespace PixelsGPU
{
	class Simulation
	{
	public:
		static constexpr unsigned int computeLocalSizeX = 32;
		static constexpr unsigned int computeLocalSizeY = 32;
		static constexpr unsigned int computeLocalSizeZ = 1;

		std::vector<Chunk> chunks;
		std::vector<PixelsGPU::Material> materials;

		float timer = 0.0f;
		int updateCount = 0;
		bool debugTest = false;
		int subUpdates = 10;

		ComputeShader* updatePixels = nullptr;
		ComputeShader* preUpdate = nullptr;
		ComputeShader* placeCircle = nullptr;
		ComputeShader* testCompute = nullptr;
		ComputeShader* testCompute2 = nullptr;
		void LoadComputeShaders();

		void InitialiseMaterials();
		void InitialiseChunks();
		void Update(float delta);

		const Chunk* getChunkAt(glm::ivec2 chunkCoords);
		Chunk& CreateChunk(glm::ivec2 chunkCoords);
		void DestroyChunk(glm::ivec2 chunkCoords);

		void SetCircleTo(glm::ivec2 pos, float radius, PixelsGPU::CellPixel cell);
		void SetCircleTo(int x, int y, float radius, PixelsGPU::CellPixel cell);

		void GUI();
	};
}
