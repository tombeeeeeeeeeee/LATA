#pragma once

#include "PixelMaterial.h"

#include "UserDefinedLiterals.h"

namespace Pixels {
	struct GpuCell {
		glm::u8vec4 colour; // vec4 for easier upload/translation
	};

	// Make sure this stays as small as reasonably can
	struct Cell {
		glm::u8vec3 colour = { 0xc5_uc, 0xde_uc, 0xe3_uc };
		bool updated : 1 = false;
		bool atRest : 1 = false;
		MatID materialID = 0;
		glm::vec2 velocity = { 0.0f, 0.0f };

		void GUI(int x, int y);
	};
}