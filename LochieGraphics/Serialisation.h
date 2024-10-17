#pragma once

#include "Maths.h"

#include <string>

#define TOML_HEADER_ONLY 0
//#define TOML_COMPILER_HAS_EXCEPTIONS 0
#include "toml.hpp"

class Serialisation {
public:
	static glm::vec2 LoadAsVec2(toml::array* toml);
	static glm::vec2 LoadAsVec2(toml::node_view<toml::node> toml);
	static glm::vec3 LoadAsVec3(toml::array* toml);
	static glm::vec3 LoadAsVec3(toml::node_view<toml::node> toml);
	static unsigned long long LoadAsUnsignedLongLong(toml::node_view<toml::node> toml);
	static unsigned long long LoadAsUnsignedLongLong(toml::node& toml);
	static std::string LoadAsString(toml::node_view<toml::node> toml);
	static std::string LoadAsString(toml::node& toml);
	static int LoadAsInt(toml::node_view<toml::node> toml);
	static int LoadAsInt(toml::node* toml);
	static int LoadAsInt(toml::node& toml);
	static float LoadAsFloat(toml::node_view<toml::node> toml);
	static float LoadAsFloat(toml::node& toml);
	static glm::quat LoadAsQuaternion(toml::node_view<toml::node> toml);
	static bool LoadAsBool(toml::node_view<toml::node> toml);

	static unsigned int LoadAsUnsignedInt(toml::node* toml);
	static unsigned int LoadAsUnsignedInt(toml::node_view<toml::node> toml);

	static toml::array SaveAsVec2(glm::vec2 vec);
	static toml::array SaveAsVec3(glm::vec3 vec);
	static std::string SaveAsUnsignedLongLong(unsigned long long n);

	static toml::array SaveAsQuaternion(glm::quat quaternion);
};







