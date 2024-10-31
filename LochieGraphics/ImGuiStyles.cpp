#include "ImGuiStyles.h"

#include "Paths.h"

#include "ImguiMathsGLM.h"

#include "Serialisation.h"

#include <fstream>
#include <ostream>

std::string ImGuiStyles::filename;

void ImGuiStyles::SetLocStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	// Sizes
	//// Main
	style->WindowPadding = ImVec2(12, 8);
	style->FramePadding = ImVec2(8, 4);
	style->ItemSpacing = ImVec2(10, 4);
	style->ItemInnerSpacing = ImVec2(4, 4);
	style->TouchExtraPadding = ImVec2(0, 0); // TODO: idk what this does
	style->IndentSpacing = 25.0f; //TODO: idk what this does
	style->ScrollbarSize = 16.0f;
	style->GrabMinSize = 17.0f;

	//// Borders
	style->WindowBorderSize = 0.f;
	style->ChildBorderSize = 0.f; //TODO: idk
	style->PopupBorderSize = 0.f;
	style->FrameBorderSize = 0.f; // This doesn't look too bad on
	style->TabBorderSize = 0.f; //TODO: idk
	style->TabBarBorderSize = 1.0f; //TODO: idk

	//// Rounding
	style->WindowRounding = 6.0f;
	style->ChildRounding = 0.f; //TODO: idk
	style->FrameRounding = 6.0f;
	style->PopupRounding = 0.f;
	style->ScrollbarRounding = 12.0f;
	style->GrabRounding = 12.0f;
	style->TabRounding = 12.f;

	//// Tables
	style->CellPadding = ImVec2(8, 2);
	style->TableAngledHeadersAngle = 50.f;

	//// Widgets
	style->WindowTitleAlign = ImVec2(0.f, 0.5f);
	style->WindowMenuButtonPosition = 1; // Right
	style->ColorButtonPosition = 1; // TODO: idk
	style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style->SelectableTextAlign = ImVec2(0.f, 0.f);
	style->SeparatorTextBorderSize = 3;
	style->SeparatorTextAlign = ImVec2(0.f, 0.5f);
	style->SeparatorTextPadding = ImVec2(20, 1);
	style->LogSliderDeadzone = 3.f; //TODO: idk

	//// Tooltips
	style->HoverFlagsForTooltipMouse = ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_Stationary; // TODO: look at flags more
	style->HoverFlagsForTooltipNav = ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay;

	//// Misc
	style->DisplaySafeAreaPadding = ImVec2(0, 0);

	// Fonts
	//TODO: load diff font?

	// Rendering
	style->AntiAliasedLines = false;
	style->AntiAliasedLinesUseTex = false;
	style->AntiAliasedFill = false;
	style->CurveTessellationTol = 1.25f; //TODO: idk
	style->CircleTessellationMaxError = 0.3f; //TODO: example
	style->Alpha = 1.0f;
	style->DisabledAlpha = 0.5f;

	// Colours
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.72f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.14f, 0.31f, 0.15f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.40f, 0.30f, 0.35f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.36f, 0.72f, 0.54f, 0.69f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.32f, 0.64f, 0.48f, 0.69f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.07f, 0.03f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.33f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.13f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.30f, 0.20f, 0.48f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.21f, 0.30f, 0.20f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.50f, 0.22f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.12f, 1.00f, 0.62f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.17f, 0.48f, 0.36f, 0.72f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.21f, 0.81f, 0.59f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.13f, 0.70f, 0.43f, 0.56f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.70f, 0.43f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.70f, 0.43f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.47f, 0.00f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.65f, 0.00f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.69f, 0.00f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.03f, 0.01f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.03f, 0.01f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.03f, 0.01f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.68f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.04f, 0.00f, 0.27f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.47f, 0.28f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.56f, 0.32f, 0.80f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.01f, 0.05f, 0.01f, 0.86f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.26f, 0.08f, 0.80f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.98f, 0.28f, 0.86f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 0.24f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	// TODO: colours idk
	// nav stuff
	// plotting stuff
	// modal window dim bg
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
	
	auto loadingColours = data["Colours"].as_array();
	for (size_t i = 0; i < loadingColours->size(); i++)
	{
		style->Colors[i] = ImguiMathsGLM::Vec4(Serialisation::LoadAsVec4(loadingColours->at(i).as_array()));
	}

	file.close();
}

void ImGuiStyles::GUI()
{
	ImGui::InputText("Filename##StyleEditor", &filename);

	if (ImGui::Button("Save##StyleEditor")) {
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##StyleEditor")) {
		Load();
	}

	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::ShowStyleEditor(style);
}
