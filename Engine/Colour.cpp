#include "Colour.h"

#include "ExtraEditorGUI.h"

Colour::Colour(toml::table& toml)
{
	Load(toml);
}

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

bool Colour::ColourPicker(std::string label, ImGuiColorEditFlags flags)
{
	std::string tag = Utilities::PointerToString(this);
	glm::vec4 tempEditColour = getAsRGBA01();
	if (ImGui::ColorPicker4((label + "##" + tag).c_str(), &tempEditColour.x))
	{
		dataType = DataType::rgba01F4;
		data.rgba01F4 = tempEditColour;
		return true;
	}
	return false;
}

toml::table Colour::Serialise() const
{
	toml::table table;
	table.insert("dataType", (int)dataType);
	switch (dataType)
	{
	case Colour::DataType::rgba01F4:
		table.insert("data", Serialisation::SaveAsVec4(data.rgba01F4));
		break;
	case Colour::DataType::rgba0255I4:
		table.insert("data", Serialisation::SaveAsU8Vec4(data.rgba0255I4));
		break;
	default:
		break;
	}
	return table;
}

void Colour::Load(toml::table& table)
{
	dataType = (Colour::DataType)Serialisation::LoadAsInt(table["dataType"]);
	switch (dataType)
	{
	case Colour::DataType::rgba01F4:
		data.rgba01F4 = Serialisation::LoadAsVec4(table["data"]);
		break;
	case Colour::DataType::rgba0255I4:
		data.rgba0255I4 = Serialisation::LoadAsU8Vec4(table["data"]);
		break;
	default:
		break;
	}
}
