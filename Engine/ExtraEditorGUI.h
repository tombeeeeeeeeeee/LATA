#pragma once

#include "EditorGUI.h"

// Needs to be included due to the template definition being here
#include "Utilities.h"
#include "UserPreferences.h"

#include <vector>
#include <string>


class ExtraEditorGUI {
public:

	static bool Mat4Input(std::string tag, glm::mat4* mat);

	static int TextSelected(ImGuiInputTextCallbackData* data);

	static bool ColourEdit3(std::string tag, glm::u8vec3& col, ImGuiColorEditFlags flags = 0);

	static bool SliderEnum(std::string label, const std::vector<std::string>& names, int* value);

	// ImGui closing/end/reverse statements shouldn't need to have to be worried about being forgotten
	// Use this to avoid having to put the unindent after
	class ScopedIndent {
	public:
		ScopedIndent();
		~ScopedIndent();
	};

	class ScopedDisable {
	public:
		ScopedDisable();
		~ScopedDisable();
	};

	class ScopedBegin {
	public:
		bool open;
		ScopedBegin(std::string name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0);
		~ScopedBegin();
		operator bool() const;
	};

	// Filter is used for the input search box
	static std::string filter;

	template <class Type, class Iter>
	static bool InputSearchBox(Iter begin, Iter end, Type** selector, std::string label, std::string tag, bool showNull = false, Type*(*createFunction) (void) = nullptr);

	// TODO: Rename
	template <class Type, class Iter>
	static bool InputSearchBox2(Iter begin, Iter end, Type** selector, std::string label, std::string tag, bool showNull, Type* (*createFunction) (void) = nullptr);
};

template<class Type, class Iter>
inline bool ExtraEditorGUI::InputSearchBox(Iter begin, Iter end, Type** selector, std::string label, std::string tag, bool showNull, Type*(*createFunction) (void))
{
	bool returnBool = false;
	std::string displayName;
	if (*selector != nullptr) {
		displayName = std::string(**selector);
	}
	else {
		displayName = "None";
	}
	std::vector <std::pair<std::string,Type*>> filteredType;

	bool textSelected = false;
	// TODO: See if the pop up can appear up better
	ImGui::InputText((label + "##" + tag).c_str(), &displayName,
		ImGuiInputTextFlags_CallbackAlways |
		ImGuiInputTextFlags_AutoSelectAll,
		ExtraEditorGUI::TextSelected, &textSelected);
	std::string popupName = (label + "##" + tag).c_str();
	if (textSelected) {
		ImGui::OpenPopup(popupName.c_str());
		if (UserPreferences::clearSearchBar) {
			filter = "";
		}
		// TODO: User prefs option to either keep or empty the search filter on new popup search
	}

	if (!ImGui::BeginPopup(popupName.c_str(), ImGuiWindowFlags_NoScrollbar)) {
		return false;
	}
	/* Popup has began*/
	ImGui::SetKeyboardFocusHere();
	ImGui::InputText(("Search##" + tag).c_str(), &ExtraEditorGUI::filter);

	// TODO: User preference file for the amount visible (the 15 v , customisable)
	ImGui::BeginChild(("List##" + tag).c_str(), ImVec2(0, 20 * 15 + 10), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY);

	for (auto& i = begin; i != end; i++)
	{
		std::string name = (std::string)(**i);
		if (Utilities::ToLower(name).find(Utilities::ToLower(ExtraEditorGUI::filter)) != std::string::npos) {
			filteredType.push_back(std::pair<std::string, Type*>{ name, * i});
		}
	}

	if (createFunction) {
		if (ImGui::MenuItem(("CREATE NEW##" + label).c_str(), "", false)) { 
			*selector = createFunction();
			returnBool = true;                                                                         
		}																							   
	}																								   
	if (showNull) {
		if (ImGui::MenuItem(("None 0##" + label).c_str(), "", false)) {
			*selector = nullptr;
			returnBool = true;
		}
	}

	for (auto& i : filteredType)
	{
		bool selected = false;
		if (i.second == *selector) {
			selected = true;
		}
		if (ImGui::MenuItem((i.first + "##" + label).c_str(), "", selected)) {
			if (*selector != i.second) {
				*selector = i.second;
				returnBool = true;
			}
		}
	}
	ImGui::EndChild();

	ImGui::EndPopup();
	return returnBool;
}


template<class Type, class Iter>
inline bool ExtraEditorGUI::InputSearchBox2(Iter begin, Iter end, Type** selector, std::string label, std::string tag, bool showNull, Type* (*createFunction) (void))
{
	std::vector<Type*> tempVector;
	for (auto& i = begin; i != end; i++)
	{
		tempVector.push_back(&(*i));
	}
	return InputSearchBox(tempVector.begin(), tempVector.end(), selector, label, tag, showNull, createFunction);
}
