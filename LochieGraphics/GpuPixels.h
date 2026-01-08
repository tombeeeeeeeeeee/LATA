#pragma once

#include "ComputeShader.h"

namespace PixelsGPU
{
	// Keep this correctly aligned with the gpu version, as thats all this is really for
	// Currently this is only used to be a size representation of the gpu version
	struct alignas(32) CellPixel // Think we would want something other than just alignas
	{
		int matID;
		int /*bool*/ movedLastUpdate;
		glm::vec2 vel;
		glm::vec2 subPos;
		glm::vec4 colour;
	};
	static_assert(sizeof(CellPixel) <= 64, "CellPixel too large, this is already more than big enough, need to pack the data better");

	class Simulation
	{
	public:
		static constexpr int chunkWidth = 128;
		static constexpr int chunkHeight = 128;

		static constexpr unsigned int computeLocalSizeX = 32;
		static constexpr unsigned int computeLocalSizeY = 32;
		static constexpr unsigned int computeLocalSizeZ = 1;


		class Chunk
		{
		public:
			// this will be the ssbo that holds all the pixel data
			unsigned int ssbo1 = 0;
			unsigned int ssbo2 = 0;
			bool readSsbo1 = false;
			bool writeSsbo1 = false;


			int x;
			int y;

			void BindCorrectReadWriteSSBOs() const;
			void SwitchReadWriteSSBOs();
			size_t CalculateSsboSize() const;

			Chunk(int _x, int _y);
			~Chunk();

			Chunk(const Chunk& _) = delete;
			Chunk& operator=(const Chunk& _) = delete;
			Chunk(Chunk&& _) = default;
			Chunk& operator =(Chunk&& _) = default;
		};

		std::vector<Chunk> chunks;

		float timer = 0.0f;
		int frameCount = 0;
		bool debugTest = false;
		int subUpdates = 10;

		ComputeShader* updatePixels = nullptr;
		ComputeShader* preUpdate = nullptr;
		ComputeShader* placeCircle = nullptr;
		ComputeShader* testCompute = nullptr;
		ComputeShader* testCompute2 = nullptr;
		void LoadComputeShaders();

		void Initialise();
		void Update(float delta);


		void SetCircleTo(glm::ivec2 pos, float radius, PixelsGPU::CellPixel cell);
		void SetCircleTo(int x, int y, float radius, PixelsGPU::CellPixel cell);

		void GUI();
	};

}
