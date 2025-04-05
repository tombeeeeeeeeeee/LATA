#pragma once

#include "PixelCell.h"

#include <array>
#include <vector>

#define PIXELS_W 100
#define PIXELS_H 100


namespace Pixels {

	std::vector<glm::ivec2> GeneratePathBetween(glm::ivec2 start, glm::ivec2 end);

	class Simulation
	{
		struct Chunk {
			std::array<std::array<Cell, PIXELS_H>, PIXELS_W> pixels;
			void Update(Simulation& pixelStuff);
		};

		// TODO: Rename, is for alternating update directions
		bool spreadTest = false;

		std::vector<Material> materialInfos;
		Simulation::Chunk chunk;

		// TODO: Once there is chunks, we might want this to not be chunk alligned so only the necessary data is uploaded
		std::array<std::array<GpuCell, PIXELS_H>, PIXELS_W> GpuPixels = {};

		bool getSpread() const;

		bool MovePixelToward(Cell& a, glm::ivec2 pos, glm::ivec2 desiredPos);
		bool SwapPixels(Cell& a, Cell& b);

		// TODO: Think about how these functions could and should exist
		void Gravity(Cell& pixel, const Material& mat, unsigned int c, unsigned int r);
		bool GravityDiagonalHelper(Cell& pixel, const Material& mat, unsigned int c, unsigned int r, bool spread);

		const Material& getMat(MatID index) const;
		Material& getNonConstMat(MatID index);

	public:
		glm::vec2 gravityForce = { 0.0f, -0.05f };
		bool testCenterGravity = false;

		std::vector<glm::ivec2> GeneratePathFromToward(const Cell& a, glm::ivec2 start, glm::ivec2 end);

		void SetSimpleMaterials();
		void PrepareDraw();
		void Update();

		unsigned int AmountOf(MatID materialID) const;

		void SetCircleToMaterial(int x, int y, float radius, MatID materialID);
		void SetDebugColours();
		// TODO: Don't like the names of these everything/all functions
		void SetEverythingTo(MatID materialID);
		void SetEverythingToColour(const glm::vec3& colour);
		void SetAllToDefaultColour();

		void AddVelocityToCircle(int x, int y, float radius, glm::vec2 vel);

		void PixelGUI(int x, int y);

		// TODO: Remove this function, just here for quicker testing for now while system underdeveloped, wherever this is called ideally this class would be handling the code there
		const GpuCell* GetGpuPixelToDrawFrom() const;
	};
}

