#pragma once

#include <string>

class ImGuiStyles
{
public:

	static std::string filename;

	static void SetLocStyle();

	static void Save();
	static void Load();

	static void GUI();
};

