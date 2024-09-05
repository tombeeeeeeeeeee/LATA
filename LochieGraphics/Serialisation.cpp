#define TOML_IMPLEMENTATION
#include "Serialisation.h"




glm::vec3 Serialisation::LoadAsVec3(toml::v3::array* toml)
{
    return glm::vec3(
        toml->at(0).value_or<float>(0.0f),
        toml->at(1).value_or<float>(0.0f),
        toml->at(2).value_or<float>(0.0f));
}

glm::vec3 Serialisation::LoadAsVec3(toml::node_view<toml::node> toml)
{
    return LoadAsVec3(toml.as_array());
}

unsigned long long Serialisation::LoadAsUnsignedLongLong(toml::v3::node_view<toml::v3::node> toml)
{
    return std::strtoull(LoadAsString(toml).c_str(), 0, 10);
}

unsigned long long Serialisation::LoadAsUnsignedLongLong(toml::node& toml)
{
    return std::strtoull(LoadAsString(toml).c_str(), 0, 10);
}

std::string Serialisation::LoadAsString(toml::v3::node_view<toml::v3::node> toml)
{
    return toml.as_string()->value_or<std::string>("");
}

std::string Serialisation::LoadAsString(toml::node& toml)
{
    return toml.as_string()->value_or<std::string>("");
}

int Serialisation::LoadAsInt(toml::v3::node_view<toml::v3::node> toml)
{
    return toml.as_integer()->value_or<int>(0);
}

float Serialisation::LoadAsFloat(toml::node& toml)
{
    return toml.as_floating_point()->value_or<float>(0.0f);
}

glm::quat Serialisation::LoadAsQuaternion(toml::node_view<toml::node> table)
{
    auto toml = table.as_array();
    return glm::quat(
        LoadAsFloat(toml->at(0)),
        LoadAsFloat(toml->at(1)),
        LoadAsFloat(toml->at(2)),
        LoadAsFloat(toml->at(3)));
}

toml::array Serialisation::SaveAsVec3(glm::vec3 vec)
{
    return toml::array(vec.x, vec.y, vec.z);
}

std::string Serialisation::SaveAsUnsignedLongLong(unsigned long long n)
{
    return std::to_string(n);
}

toml::array Serialisation::SaveAsQuaternion(glm::quat quaternion)
{
    return toml::array(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
}









