#pragma once


#include "Maths.h"

#include <string>

#define TOML_HEADER_ONLY 1
#include "toml.hpp"


class Serialisation {
public:
	static glm::vec2 LoadAsVec2(toml::v3::array* toml);
	static glm::vec2 LoadAsVec2(toml::node_view<toml::node> toml);
	// TODO: Pretty sure the v3 isn't needed, remove
	static glm::vec3 LoadAsVec3(toml::v3::array* toml);
	static glm::vec3 LoadAsVec3(toml::node_view<toml::node> toml);
	static unsigned long long LoadAsUnsignedLongLong(toml::v3::node_view<toml::v3::node> toml);
	static unsigned long long LoadAsUnsignedLongLong(toml::node& toml);
	static std::string LoadAsString(toml::v3::node_view<toml::v3::node> toml);
	static std::string LoadAsString(toml::node& toml);
	static int LoadAsInt(toml::v3::node_view<toml::v3::node> toml);
	static float LoadAsFloat(toml::v3::node_view<toml::v3::node> toml);
	static float LoadAsFloat(toml::node& toml);
	static glm::quat LoadAsQuaternion(toml::node_view<toml::node> toml);
	static bool LoadAsBool(toml::node_view<toml::node> toml);


	static toml::array SaveAsVec2(glm::vec2 vec);
	static toml::array SaveAsVec3(glm::vec3 vec);
	static std::string SaveAsUnsignedLongLong(unsigned long long n);

	static std::string SaveAsUnsignedInt(unsigned int n);

	static toml::array SaveAsQuaternion(glm::quat quaternion);
};







