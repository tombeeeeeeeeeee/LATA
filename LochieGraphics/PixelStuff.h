#pragma once

#include "Maths.h"
#include "UserDefinedLiterals.h"

#include <string>
#include <vector>
#include <array>

#define PIXELS_W 100
#define PIXELS_H 100

typedef unsigned short MatID;

// TODO: temp class name, what should it be called? Pixels?
class PixelStuff {
private:
	enum MaterialFlags {
		neverUpdate = 0, // Prob just doesn't need to exist? (Could exist as a combo of/lack of flags?)
		gravity = 1 << 0
	};

	// Make sure this stays as small as reasonably can
	struct PixelData {
		glm::u8vec3 colour = { 0xc5_uc, 0xde_uc, 0xe3_uc };
		bool updated = false;
		MatID materialID = 0;
		glm::vec2 velocity = { 0.0f, 0.0f };

		void GUI();
	};

	struct MaterialInfo {
		std::string name;
		glm::u8vec3 defaultColour;
		unsigned int flags;
		float density = 0; // Would we want this to be a float or an int?
		MaterialInfo(std::string _name, glm::u8vec3(_colour), float _density, unsigned int _flags);

		void GUI();
	};

	struct Chunk {
		std::array<std::array<PixelData, PIXELS_H>, PIXELS_W> pixels;
		void Update(PixelStuff& pixelStuff);
	};

public:
	// TODO: This is only public as this isn't drawing it, confirm what is and if this can be private
	// This is intended to always be the same as the version for the shader
	struct GpuPixelData {
		glm::u8vec4 colour; // vec4 for easier upload/translation
	};

private:
	// TODO: Rename, is for alternating update directions
	bool spreadTest = false;
	std::vector<MaterialInfo> materialInfos;
	PixelStuff::Chunk chunk;
	// TODO: Once there is chunks, we might want this to not be chunk alligned so only the necessary data is uploaded
	std::array<std::array<GpuPixelData, PIXELS_H>, PIXELS_W> GpuPixels = {};

	bool getSpread() const;

	bool MovePixelToward(PixelData& a, glm::ivec2 pos, glm::ivec2 desiredPos);
	bool SwapPixels(PixelData& a, PixelData& b);

	// TODO: Think about how these functions could and should exist
	void Gravity(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r);
	bool GravityDiagonalHelper(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r, bool spread);

	const MaterialInfo& getMat(MatID index) const;
	MaterialInfo& getNonConstMat(MatID index);

public:
	glm::vec2 gravityForce = { 0.0f, -0.05f };
	bool testCenterGravity = false;

	static std::vector<glm::ivec2> GeneratePathBetween(glm::ivec2 start, glm::ivec2 end);
	std::vector<glm::ivec2> GeneratePathFromToward(const PixelData& a, glm::ivec2 start, glm::ivec2 end);

	void SetSimpleMaterials();
	void PrepareDraw();
	void UpdateSim();

	unsigned int AmountOf(MatID materialID) const;
	
	void SetCircleToMaterial(int x, int y, float radius, MatID materialID);
	void SetDebugColours();
	// TODO: Don't like the names of these everything/all functions
	void SetEverythingTo(MatID materialID);
	void SetEverythingToColour(const glm::vec3& colour);
	void SetAllToDefaultColour();

	void AddVelocityTo(int x, int y, float radius, glm::vec2 vel);

	void PixelGUI(int x, int y);

	// TODO: Remove this function, just here for quicker testing for now while system underdeveloped, wherever this is called ideally this class would be handling the code there
	const PixelStuff::GpuPixelData* GetGpuPixelToDrawFrom() const;
};

