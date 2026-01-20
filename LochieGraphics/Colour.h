#pragma once

#include "Maths.h"

#include "Serialisation.h"

#include "imgui.h"

class Colour
{
public:

	static constexpr unsigned int U32RShift = 24;
	static constexpr unsigned int U32GShift = 16;
	static constexpr unsigned int U32BShift = 8;
	static constexpr unsigned int U32AShift = 0;


	using rgbF4_t = glm::vec4;
	using rgbI4_t = glm::u8vec4;

	enum class DataType
	{
		rgba01F4,
		rgba0255I4,
	};
	union Data
	{
		rgbF4_t rgba01F4;
		rgbI4_t rgba0255I4;
	};
	Data data;
	DataType dataType;

	constexpr Colour(Data _data, DataType _dataType) :
		data(_data),
		dataType(_dataType)
	{
	}

	constexpr Colour() :
		data(glm::u8vec4(0, 0, 0, 1)),
		dataType(DataType::rgba0255I4)
	{
	}

	Colour(toml::table& toml);

	static rgbF4_t RGBA01FromRGBA0255(rgbI4_t input);
	static Colour::rgbI4_t RGBA0255FromRGBA01(rgbF4_t input);
	static uint32_t RGBAU32FromRGBA0255(rgbI4_t input);
	static uint32_t RGBAU32FromRGBA01(rgbF4_t input);

	rgbF4_t getAsRGBA01() const;
	Colour::rgbI4_t getAsRGBA0255() const;
	uint32_t getAsRGBAU32() const;

	bool ColourPicker(std::string label, ImGuiColorEditFlags flags = 0);

	toml::table Serialise() const;
	void Load(toml::table& toml);
};

