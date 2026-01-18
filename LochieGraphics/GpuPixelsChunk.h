#pragma once

#include "Maths.h"

namespace PixelsGPU
{
	static constexpr int chunkWidth = 256;
	static constexpr int chunkHeight = 256;

	class Chunk
	{
	public:

		static constexpr int offsetIndexSBBO = 3;
		static constexpr int centreIndexSSBO = offsetIndexSBBO + 1;

		static constexpr int orderedNearIndexSSBO[8] =
		{
			offsetIndexSBBO + 2,
			offsetIndexSBBO + 3,
			offsetIndexSBBO + 4,
			offsetIndexSBBO + 5,
			offsetIndexSBBO + 6,
			offsetIndexSBBO + 7,
			offsetIndexSBBO + 8,
			offsetIndexSBBO + 9,
		};

		static constexpr glm::ivec2 orderedNearChunksLocalOffsets[8] =
		{
			glm::ivec2(-1, 0),
			glm::ivec2(0, 1),
			glm::ivec2(1, 0),
			glm::ivec2(0, -1),
			glm::ivec2(-1, 1),
			glm::ivec2(1, 1),
			glm::ivec2(-1, -1),
			glm::ivec2(1, -1),
		};

		// this will be the ssbo that holds all the pixel data
		unsigned int ssbo1 = 0;
		//unsigned int ssbo2 = 0;
		bool readSsboFirst = false;

		glm::ivec2 coords;

		void BindSSBO(int index) const;
		void SwitchReadWriteSSBOs();
		size_t CalculateSsboSize() const;

		void Initialise();
		void Deinitalise();

		Chunk(glm::ivec2 _coords);
		~Chunk();

		Chunk(const Chunk& _) = delete;
		Chunk& operator=(const Chunk& _) = delete;
		Chunk(Chunk&& _) = default;
		Chunk& operator =(Chunk&& _) = default;
	};
}
