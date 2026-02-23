#include "ImGuiStyles.h"

#include "Paths.h"

#include "ImguiMathsGLM.h"
#include "ExtraEditorGUI.h"

#include "Serialisation.h"

#include <fstream>
#include <ostream>
#include <filesystem>

bool ImGuiStyles::saveAs = false;
std::string ImGuiStyles::filename;

void ImGuiStyles::SetLocStyle()
{
	filename = "LocStyle";
	Load();
}

// TODO: not saving loading
// HoverFlagsForTooltipMouse
// HoverFlagsForTooltipNav

void ImGuiStyles::Save()
{
	if (filename == "") { return; }

	std::ofstream file(Paths::stylesPath + filename + Paths::styleExtension);

	ImGuiStyle* style = &ImGui::GetStyle();

	toml::array colours;
	for (size_t i = 0; i < ImGuiCol_COUNT; i++)
	{
		colours.push_back(Serialisation::SaveAsVec4(ImguiMathsGLM::Vec4(style->Colors[i])));
	}

	toml::table table{
		{ "Alpha", style->Alpha },
		{ "DisabledAlpha", style->DisabledAlpha },
		{ "WindowPadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->WindowPadding)) },
		{ "WindowRounding", style->WindowRounding },
		{ "WindowBorderSize", style->WindowBorderSize },
		{ "WindowMinSize", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->WindowMinSize)) },
		{ "WindowTitleAlign", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->WindowTitleAlign)) },
		{ "WindowMenuButtonPosition", (int)style->WindowMenuButtonPosition },
		{ "ChildRounding", style->ChildRounding },
		{ "ChildBorderSize", style->ChildBorderSize },
		{ "PopupRounding", style->PopupRounding },
		{ "PopupBorderSize", style->PopupBorderSize },
		{ "FramePadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->FramePadding)) },
		{ "FrameRounding", style->FrameRounding },
		{ "FrameBorderSize", style->FrameBorderSize },
		{ "ItemSpacing", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->ItemSpacing)) },
		{ "ItemInnerSpacing", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->ItemInnerSpacing)) },
		{ "CellPadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->CellPadding)) },
		{ "TouchExtraPadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->TouchExtraPadding)) },
		{ "IndentSpacing", style->IndentSpacing },
		{ "ColumnsMinSpacing", style->ColumnsMinSpacing },
		{ "ScrollbarSize", style->ScrollbarSize },
		{ "ScrollbarRounding", style->ScrollbarRounding },
		{ "GrabMinSize", style->GrabMinSize },
		{ "GrabRounding", style->GrabRounding },
		{ "LogSliderDeadzone", style->LogSliderDeadzone },
		{ "TabRounding", style->TabRounding },
		{ "TabBorderSize", style->TabBorderSize },
		{ "TabMinWidthForCloseButton", style->TabMinWidthForCloseButton },
		{ "TabBarBorderSize", style->TabBarBorderSize },
		{ "TableAngledHeadersAngle", style->TableAngledHeadersAngle },
		{ "ColorButtonPosition", (int)style->ColorButtonPosition },
		{ "ButtonTextAlign", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->ButtonTextAlign)) },
		{ "SelectableTextAlign", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->SelectableTextAlign)) },
		{ "SeparatorTextBorderSize", style->SeparatorTextBorderSize },
		{ "SeparatorTextAlign", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->SeparatorTextAlign)) },
		{ "SeparatorTextPadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->SeparatorTextPadding)) },
		{ "DisplayWindowPadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->DisplayWindowPadding)) },
		{ "DisplaySafeAreaPadding", Serialisation::SaveAsVec2(ImguiMathsGLM::Vec2(style->DisplaySafeAreaPadding)) },
		{ "DockingSeparatorSize", style->DockingSeparatorSize },
		{ "MouseCursorScale", style->MouseCursorScale },
		{ "AntiAliasedLines", style->AntiAliasedLines },
		{ "AntiAliasedLinesUseTex", style->AntiAliasedLinesUseTex },
		{ "AntiAliasedFill", style->AntiAliasedFill },
		{ "CurveTessellationTol", style->CurveTessellationTol },
		{ "CircleTessellationMaxError", style->CircleTessellationMaxError },
		{ "Colours", colours },
		{ "Font", std::string(ImGui::GetIO().FontDefault->GetDebugName())},
	};

	file << table << '\n';

	file.close();
}

void ImGuiStyles::Load()
{
	std::ifstream file(Paths::stylesPath + filename + Paths::styleExtension);

	if (!file) {
		return;
	}

	toml::table data = toml::parse(file);

	ImGuiStyle* style = &ImGui::GetStyle();


	style->Alpha = Serialisation::LoadAsFloat(data["Alpha"]);
	style->DisabledAlpha = Serialisation::LoadAsFloat(data["DisabledAlpha"]);
	style->WindowPadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["WindowPadding"]));
	style->WindowRounding = Serialisation::LoadAsFloat(data["WindowRounding"]);
	style->WindowBorderSize = Serialisation::LoadAsFloat(data["WindowBorderSize"]);
	style->WindowMinSize = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["WindowMinSize"]));
	style->WindowTitleAlign = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["WindowTitleAlign"]));
	style->WindowMenuButtonPosition = Serialisation::LoadAsInt(data["WindowMenuButtonPosition"]);
	style->ChildRounding = Serialisation::LoadAsFloat(data["ChildRounding"]);
	style->ChildBorderSize = Serialisation::LoadAsFloat(data["ChildBorderSize"]);
	style->PopupRounding = Serialisation::LoadAsFloat(data["PopupRounding"]);
	style->PopupBorderSize = Serialisation::LoadAsFloat(data["PopupBorderSize"]);
	style->FramePadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["FramePadding"]));
	style->FrameRounding = Serialisation::LoadAsFloat(data["FrameRounding"]);
	style->FrameBorderSize = Serialisation::LoadAsFloat(data["FrameBorderSize"]);
	style->ItemSpacing = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["ItemSpacing"]));
	style->ItemInnerSpacing = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["ItemInnerSpacing"]));
	style->CellPadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["CellPadding"]));
	style->TouchExtraPadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["TouchExtraPadding"]));
	style->IndentSpacing = Serialisation::LoadAsFloat(data["IndentSpacing"]);
	style->ColumnsMinSpacing = Serialisation::LoadAsFloat(data["ColumnsMinSpacing"]);
	style->ScrollbarSize = Serialisation::LoadAsFloat(data["ScrollbarSize"]);
	style->ScrollbarRounding = Serialisation::LoadAsFloat(data["ScrollbarRounding"]);
	style->GrabMinSize = Serialisation::LoadAsFloat(data["GrabMinSize"]);
	style->GrabRounding = Serialisation::LoadAsFloat(data["GrabRounding"]);
	style->LogSliderDeadzone = Serialisation::LoadAsFloat(data["LogSliderDeadzone"]);
	style->TabRounding = Serialisation::LoadAsFloat(data["TabRounding"]);
	style->TabBorderSize = Serialisation::LoadAsFloat(data["TabBorderSize"]);
	style->TabMinWidthForCloseButton = Serialisation::LoadAsFloat(data["TabMinWidthForCloseButton"]);
	style->TabBarBorderSize = Serialisation::LoadAsFloat(data["TabBarBorderSize"]);
	style->TableAngledHeadersAngle = Serialisation::LoadAsFloat(data["TableAngledHeadersAngle"]);
	style->ColorButtonPosition = Serialisation::LoadAsInt(data["ColorButtonPosition"]);
	style->ButtonTextAlign = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["ButtonTextAlign"]));
	style->SelectableTextAlign = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["SelectableTextAlign"]));
	style->SeparatorTextBorderSize = Serialisation::LoadAsFloat(data["SeparatorTextBorderSize"]);
	style->SeparatorTextAlign = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["SeparatorTextAlign"]));
	style->SeparatorTextPadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["SeparatorTextPadding"]));
	style->DisplayWindowPadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["DisplayWindowPadding"]));
	style->DisplaySafeAreaPadding = ImguiMathsGLM::Vec2(Serialisation::LoadAsVec2(data["DisplaySafeAreaPadding"]));
	style->DockingSeparatorSize = Serialisation::LoadAsFloat(data["DockingSeparatorSize"]);
	style->MouseCursorScale = Serialisation::LoadAsFloat(data["MouseCursorScale"]);
	style->AntiAliasedLines = Serialisation::LoadAsBool(data["AntiAliasedLines"]);
	style->AntiAliasedLinesUseTex = Serialisation::LoadAsBool(data["AntiAliasedLinesUseTex"]);
	style->AntiAliasedFill = Serialisation::LoadAsBool(data["AntiAliasedFill"]);
	style->CurveTessellationTol = Serialisation::LoadAsFloat(data["CurveTessellationTol"]);
	style->CircleTessellationMaxError = Serialisation::LoadAsFloat(data["CircleTessellationMaxError"]);
	
	std::string fontName = Serialisation::LoadAsString(data["Font"]);
	for (ImFont* font : ImGui::GetIO().Fonts->Fonts)
	{
		if (font->GetDebugName() == fontName) {
			ImGui::GetIO().FontDefault = font;
			break;
		}
	}
	
	auto loadingColours = data["Colours"].as_array();
	for (size_t i = 0; i < loadingColours->size(); i++)
	{
		style->Colors[i] = ImguiMathsGLM::Vec4(Serialisation::LoadAsVec4(loadingColours->at(i).as_array()));
	}

	file.close();
}

void ImGuiStyles::GUI()
{
	if (Selector()) {
		UserPreferences::defaultStyleLoad = filename;
		UserPreferences::Save();
	}

	if (ImGui::Button("Save##StyleEditor")) {
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##StyleEditor")) {
		Load();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save As##StyleEditor")) {
		saveAs = true;
	}

	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::ShowStyleEditor(style);

	SaveAsGUI();
}

bool ImGuiStyles::Selector()
{
	// TODO: There are quite a few selectors for specifically files, make a function for that
	std::vector<std::string> paths;
	std::vector<std::string*> pathPointers;

	std::string* selected = nullptr;

	for (auto& i : std::filesystem::directory_iterator(Paths::stylesPath)) {

		paths.push_back(i.path().generic_string().substr(Paths::stylesPath.size()));
		if (paths.back().substr(paths.back().size() - Paths::styleExtension.size()) != Paths::styleExtension) {
			paths.erase(--paths.end());
			continue;
		}
		paths.back() = paths.back().substr(0, paths.back().size() - Paths::styleExtension.size());
	}
	pathPointers.reserve(paths.size());
	for (size_t i = 0; i < paths.size(); i++)
	{
		pathPointers.push_back(&paths[i]);
		if (paths[i] == filename) {
			selected = &paths[i];
		}
	}

	if (ExtraEditorGUI::InputSearchBox(pathPointers.begin(), pathPointers.end(), &selected, "Style Filename", "StyleSelector")) {
		filename = *selected;
		Load();
		return true;
	}
	return false;
}

void ImGuiStyles::SaveAsGUI()
{
	if (saveAs)
	{
		ImGui::OpenPopup("Save Style As");
		saveAs = false;
	}
	if (ImGui::BeginPopupModal("Save Style As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::InputText("File ", &filename);

		if (ImGui::Button("Save"))
		{
			if (filename != "") {
				std::ofstream file(Paths::stylesPath + filename + Paths::styleExtension);
				Save();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
