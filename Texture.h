#pragma once

#include <unordered_map>
#include <string>


class Texture
{
public:
	enum class Type {
		diffuse,
		specular,
		normal,
		height,
		emission
	};
	static const std::unordered_map<Type, std::string> TypeNames;

	unsigned int ID;
	Type type;
	Texture();
};

