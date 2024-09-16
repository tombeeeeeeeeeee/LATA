#define TOML_IMPLEMENTATION 1
#include "Serialisation.h"




glm::vec2 Serialisation::LoadAsVec2(toml::v3::array* toml)
{
    // TODO: Show error message

    if (!toml) { return { 0.0f, 0.0f }; }
    return glm::vec2(
        toml->at(0).value_or<float>(0.0f),
        toml->at(1).value_or<float>(0.0f));
}

glm::vec2 Serialisation::LoadAsVec2(toml::node_view<toml::node> toml)
{
    return LoadAsVec2(toml.as_array());
}

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
    if (!toml) { return 0.0f; }
    return toml.as_integer()->value_or<int>(0);
}

int Serialisation::LoadAsInt(toml::node* toml)
{
    if (!toml) { return 0.0f; }

    return toml->as_integer()->value_or<int>(0);
}

int Serialisation::LoadAsInt(toml::node& toml)
{
    return toml.as_integer()->value_or<int>(0);
}

float Serialisation::LoadAsFloat(toml::v3::node_view<toml::v3::node> toml)
{
    // TODO: Show error message

    if (!toml) { return 0.0f; }
    return toml.as_floating_point()->value_or<float>(0.0f);
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

bool Serialisation::LoadAsBool(toml::node_view<toml::node> toml)
{
    // TODO: Show error message
    if (!toml) { return false; }
    return toml.as_boolean()->value_or<bool>(false);
}

toml::array Serialisation::SaveAsVec2(glm::vec2 vec)
{
    return toml::array(vec.x, vec.y);
}

toml::array Serialisation::SaveAsVec3(glm::vec3 vec)
{
    return toml::array(vec.x, vec.y, vec.z);
}

std::string Serialisation::SaveAsUnsignedLongLong(unsigned long long n)
{
    return std::to_string(n);
}

std::string Serialisation::SaveAsUnsignedInt(unsigned int n)
{
    return std::to_string(n);
}

toml::array Serialisation::SaveAsQuaternion(glm::quat quaternion)
{
    return toml::array(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
}









