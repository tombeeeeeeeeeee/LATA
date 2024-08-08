#pragma once

#include "toml.hpp"

#include "Maths.h"

#include <string>

class Serialisation {
public:
	static glm::vec3 LoadAsVec3(toml::v3::array* toml);
	static unsigned long long LoadAsUnsignedLongLong(toml::v3::node_view<toml::v3::node> toml);
	static std::string LoadAsString(toml::v3::node_view<toml::v3::node> toml);
	static int LoadAsInt(toml::v3::node_view<toml::v3::node> toml);

};


