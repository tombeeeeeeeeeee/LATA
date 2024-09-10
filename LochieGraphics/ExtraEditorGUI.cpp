#include "ExtraEditorGUI.h"

std::string ExtraEditorGUI::filter = "";

int ExtraEditorGUI::TextSelected(ImGuiInputTextCallbackData* data)
{
	// TODO: Use re intererpept cast
	*((bool*)data->UserData) = true;
	return 0;
}