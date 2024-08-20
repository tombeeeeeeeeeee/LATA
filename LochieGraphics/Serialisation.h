#pragma once


#include "Maths.h"

#include <string>

#define TOML_HEADER_ONLY 0
#include "toml.hpp"


class Serialisation {
public:
	static glm::vec3 LoadAsVec3(toml::v3::array* toml);
	static unsigned long long LoadAsUnsignedLongLong(toml::v3::node_view<toml::v3::node> toml);
	static std::string LoadAsString(toml::v3::node_view<toml::v3::node> toml);
	static int LoadAsInt(toml::v3::node_view<toml::v3::node> toml);

	static toml::array SaveAsVec3(glm::vec3 vec);
	static std::string SaveAsUnsignedLongLong(unsigned long long n);
};







