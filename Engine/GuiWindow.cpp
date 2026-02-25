#include "GuiWindow.h"

#include "ExtraEditorGUI.h"
#include "GUI.h"

GuiWindow::GuiWindow(GUI& gui, std::string menuName) :
	guiMenuButtonName(menuName)
{
	gui.RegisterGuiWindow(this);
}

bool GuiWindow::MainMenuItem()
{
	return ExtraEditorGUI::MainMenuItem(GUI::mainMenuWindowName, guiMenuButtonName, &showGuiWindow);
}
