#pragma once

#include <string>

class GUI;

class GuiWindow
{
public:
	GuiWindow(GUI& gui, std::string menuName);

	bool MainMenuItem();

	virtual bool DoGuiWindow() = 0;
	bool showGuiWindow = false;
	std::string guiMenuButtonName;
};
