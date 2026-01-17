#include "Colour.h"

#include "ExtraEditorGUI.h"

Colour::rgbF4_t Colour::RGBA01FromRGBA0255(Colour::rgbI4_t input)
{
	return glm::vec4(input) / 255.0f;
}

Colour::rgbI4_t Colour::RGBA0255FromRGBA01(Colour::rgbF4_t input)
{
	return Colour::rgbI4_t(input * 255.0f);
}

uint32_t Colour::RGBAU32FromRGBA0255(Colour::rgbI4_t input)
{
	return (input.x << U32RShift) + (input.y << U32GShift) + (input.z << U32BShift) + (input.w << U32AShift);
}

uint32_t Colour::RGBAU32FromRGBA01(rgbF4_t input)
{
	return RGBAU32FromRGBA0255(RGBA0255FromRGBA01(input));
}

Colour::rgbF4_t Colour::getAsRGBA01() const
{
	switch (dataType)
	{
	case Colour::DataType::rgba01F4:
		return data.rgba01F4;
		break;
	case Colour::DataType::rgba0255I4:
		return RGBA01FromRGBA0255(data.rgba0255I4);
		break;
	default:
		break;
	}
}

Colour::rgbI4_t Colour::getAsRGBA0255() const
{
	switch (dataType)
	{
	case Colour::DataType::rgba01F4:
		return RGBA0255FromRGBA01(data.rgba01F4);
		break;
	case Colour::DataType::rgba0255I4:
		return data.rgba0255I4;
		break;
	default:
		break;
	}
}

uint32_t Colour::getAsRGBAU32() const
{
	switch (dataType)
	{
	case Colour::DataType::rgba01F4:
		return RGBAU32FromRGBA01(data.rgba01F4);
		break;
	case Colour::DataType::rgba0255I4:
		return RGBAU32FromRGBA0255(data.rgba0255I4);
		break;
	default:
		break;
	}
}
