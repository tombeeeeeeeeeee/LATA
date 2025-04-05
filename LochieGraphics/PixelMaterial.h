#pragma once

#include "Maths.h"

#include <string>

typedef unsigned short MatID;

namespace Pixels {
	enum MaterialFlags {
		neverUpdate = 0, // Prob just doesn't need to exist? (Could exist as a combo of/lack of flags?)
		gravity = 1 << 0
	};

	struct Material {
		std::string name;
		glm::u8vec3 defaultColour;
		unsigned int flags;
		float density = 0; // Would we want this to be a float or an int?
		Material(std::string _name, glm::u8vec3(_colour), float _density, unsigned int _flags);

		void GUI();
	};
}
