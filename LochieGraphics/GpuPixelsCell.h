#pragma once

#include "Maths.h"

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
		unsigned int colour;
	};
	static_assert(sizeof(CellPixel) <= 64, "CellPixel too large, this is already more than big enough, need to pack the data better");
}

