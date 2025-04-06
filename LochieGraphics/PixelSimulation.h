#pragma once

#include "PixelCell.h"

#include "hashFNV1A.h"

#include <array>
#include <vector>
#include <unordered_map>


namespace Pixels {

	std::vector<glm::ivec2> GeneratePathBetween(glm::ivec2 start, glm::ivec2 end);

	constexpr short chunkWidth = 64;
	constexpr short chunkHeight = 64;

	class Simulation
	{
		struct Chunk {

			const int x;
			const int y;

			std::array<std::array<Cell, chunkHeight>, chunkWidth> cells;

			unsigned int ssbo;

			bool draw = true;
			bool drawVelocity = false;

			void Update(Simulation& pixelStuff);

			Cell& getLocal(int x, int y);

			Chunk(int _x, int _y);

			void SetDebugColours();

			void PrepareDraw();

			const GpuCell* GetGpuPixelToDrawFrom() const;

			// TODO: Once there is chunks, we might want this to not be chunk alligned so only the necessary data is uploaded
			// TODO: This should not be chunk width/height, should be able to change
			std::array<std::array<GpuCell, chunkHeight>, chunkWidth> GpuCells = {};
		};

		// TODO: Rename, is for alternating update directions
		bool spreadTest = false;

		std::vector<Material> materialInfos;

		Cell theEdge;

		unsigned short maxChunks = 32;

		std::vector<Simulation::Chunk> chunks;
		std::unordered_map<std::pair<int, int>, Simulation::Chunk*, hashFNV1A> chunkLookup;

		Chunk& AddChunk(int x, int y);

		Cell& getGlobal(int cellX, int cellY);

		bool getSpread() const;

		bool MovePixelToward(Cell& a, glm::ivec2 pos, glm::ivec2 desiredPos);
		void setCell(int x, int y, MatID matID);
		bool SwapPixels(Cell& a, Cell& b);

		// TODO: Think about how these functions could and should exist
		void Gravity(Cell& pixel, const Material& mat, int x, int y);
		bool GravityDiagonalHelper(Cell& pixel, const Material& mat, unsigned int c, unsigned int r, bool spread);

		const Material& getMat(MatID index) const;
		Material& getNonConstMat(MatID index);
	public:
		glm::vec2 gravityForce = { 0.0f, -0.05f };
		bool testCenterGravity = false;

		std::vector<glm::ivec2> GeneratePathFromToward(const Cell& a, glm::ivec2 start, glm::ivec2 end);

		void SetSimpleMaterials();
		void PrepareDraw(int left, int down);
		void Update();

		unsigned int AmountOf(MatID materialID) const;

		void SetCircleToMaterial(int cx, int cy, float radius, MatID materialID);
		// TODO: Don't like the names of these everything/all functions
		void SetEverythingTo(MatID materialID);
		void SetEverythingToColour(const glm::vec3& colour);
		void SetAllToDefaultColour();

		void AddVelocityToCircle(int x, int y, float radius, glm::vec2 vel);

		void PixelGUI(int x, int y);

		bool isCellAt(int x, int y);

		int getChunkCount();

		const std::vector<Chunk>& getChunks() const;

		void SetDebugColours();

		void SetDrawVelocity(bool value);

		Simulation();
	};
}

