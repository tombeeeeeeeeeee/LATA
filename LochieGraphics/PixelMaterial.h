#pragma once

#include "Maths.h"

#include <string>

typedef unsigned short MatID;

namespace Pixels {
	enum MaterialFlags {
		neverUpdate = 1 << 0,
		gravity = 2 << 0
	};

	struct Material {
		std::string name;
		glm::u8vec3 defaultColour;
		float halfAngleSpread = PI / 4;
		unsigned int flags;
		float density = 0; // Would we want this to be a float or an int?
		Material(std::string _name, glm::u8vec3(_colour), float _density, unsigned int _flags);

		void GUI();
	};
}
