#pragma once

#include "EditorGUI.h"

#include "Utilities.h"

#include <vector>
#include <string>

class ExtraEditorGUI {

	static int TextSelected(ImGuiInputTextCallbackData* data);

	static std::string filter;

	template <class Type, class Iter>
	bool InputSearchBox(Iter begin, Iter end, Type selector, std::string label, std::string tag, bool showNull);
}



template<class Type, class Iter>
bool ExtraEditorGUI::InputSearchBox(Iter begin, Iter end, Type selector, std::string label, std::string tag, bool showNull)
{
	bool returnBool;
	std::string displayName;
	if (selector != nullptr) {
		displayName = std::string(*selector);
	}
	else {
		displayName = "None";
	}
	std::vector <std::pair<std::string, Type>> filteredType;

	bool textSelected = false;
	/*TODO: Text based input instead of button prompt, the pop up should appear while typing*/
	ImGui::InputText((label + "##" + tag).c_str(), &displayName,
		ImGuiInputTextFlags_CallbackAlways |
		ImGuiInputTextFlags_AutoSelectAll,
		ExtraEditorGUI::TextSelected, &textSelected);
	std::string popupName = (label + "##" + tag).c_str();
	if (textSelected) {
		ImGui::OpenPopup(popupName.c_str(), ImGuiPopupFlags_NoReopen);
	}

	if (!ImGui::BeginPopup(popupName.c_str())) {
		return false;
	}
	/* Popup has began*/
	ImGui::SetKeyboardFocusHere();
	ImGui::InputText(("Search##" + tag).c_str(), &ExtraEditorGUI::filter);

	for (auto& i = begin; i != end; i++)
	{
		std::string name = (std::string)(**i);
		if (Utilities::ToLower(name).find(Utilities::ToLower(ExtraEditorGUI::filter)) != std::string::npos) {
			filteredType.push_back(std::pair<std::string, Type>{ name, * i});
		}
	}

	//if (showCreateButton) {																			   
	//	if (ImGui::MenuItem(("CREATE NEW " + std::string(#type) + "##" + label).c_str(), "", false)) { 
	//		newConstructorLine                                                                         
	//		returnBool = true;                                                                         
	//	}																							   
	//}																								   
	if (showNull) {
		if (ImGui::MenuItem(("None 0##" + label).c_str(), "", false)) {
			selector = nullptr;
			returnBool = true;
		}
	}

	for (auto& i : filteredType)
	{
		bool selected = false;
		if (i.second == selector) {
			selected = true;
		}
		if (ImGui::MenuItem((i.first + "##" + label).c_str(), "", selected)) {
			if (selector != i.second) {
				selector = i.second;
				returnBool = true;
			}
		}
	}

	ImGui::EndPopup();
	return returnBool;
}
