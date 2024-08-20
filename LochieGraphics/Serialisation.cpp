#define TOML_IMPLEMENTATION
#include "Serialisation.h"




glm::vec3 Serialisation::LoadAsVec3(toml::v3::array* toml)
{
    return glm::vec3(
        toml->at(0).value_or<float>(0.0f),
        toml->at(1).value_or<float>(0.0f),
        toml->at(2).value_or<float>(0.0f));
}

unsigned long long Serialisation::LoadAsUnsignedLongLong(toml::v3::node_view<toml::v3::node> toml)
{
    return std::strtoull(LoadAsString(toml).c_str(), 0, 10);
}

std::string Serialisation::LoadAsString(toml::v3::node_view<toml::v3::node> toml)
{
    return toml.as_string()->value_or<std::string>("");
}

int Serialisation::LoadAsInt(toml::v3::node_view<toml::v3::node> toml)
{
    return toml.as_integer()->value_or<int>(0);
}

toml::array Serialisation::SaveAsVec3(glm::vec3 vec)
{
    return toml::array(vec.x, vec.y, vec.z);
}

std::string Serialisation::SaveAsUnsignedLongLong(unsigned long long n)
{
    return std::to_string(n);
}









