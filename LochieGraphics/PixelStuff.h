#pragma once

#include "Maths.h"
#include "UserDefinedLiterals.h"

#include <string>
#include <vector>
#include <array>

#define PIXELS_W 64
#define PIXELS_H 64

// TODO: temp class name, what should it be called? Pixels?
class PixelStuff {
private:
	enum MaterialFlags {
		neverUpdate = 0, // Prob just doesn't need to exist? (Could exist as a combo of/lack of flags?)
		gravity = 1 << 0
	};

	struct PixelData {
		glm::u8vec3 colour = { 0xc5_uc, 0xde_uc, 0xe3_uc };
		bool updated = false;
		unsigned int materialID = 0;

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

	bool SwapPixels(PixelData& a, PixelData& b);

	// TODO: Think about how these functions could and should exist
	void Gravity(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r);
	bool GravityDiagonalHelper(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r, bool spread);

	const MaterialInfo& getMat(unsigned int index) const;
	MaterialInfo& getNonConstMat(unsigned int index);

public:
	void SetSimpleMaterials();
	void PrepareDraw();
	void UpdateSim();

	unsigned int AmountOf(unsigned int materialID) const;
	
	void SetCircleToMaterial(int x, int y, float radius, unsigned int materialID);
	void SetDebugColours();
	// TODO: Don't like the names of these everything/all functions
	void SetEverythingTo(unsigned int materialID);
	void SetEverythingToColour(const glm::vec3& colour);
	void SetAllToDefaultColour();

	void PixelGUI(int x, int y);

	// TODO: Remove this function, just here for quicker testing for now while system underdeveloped, wherever this is called ideally this class would be handling the code there
	const PixelStuff::GpuPixelData* GetGpuPixelToDrawFrom() const;
};

