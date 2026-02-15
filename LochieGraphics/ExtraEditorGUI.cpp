#include "ExtraEditorGUI.h"

#include "ImGuizmo.h"

std::string ExtraEditorGUI::filter = "";

bool ExtraEditorGUI::Mat4Input(std::string tag, glm::mat4 * mat)
{
	glm::vec3 pos = {};
	glm::vec3 rot = {};
	glm::vec3 scl = {};

	// TODO: Create and use own function
	ImGuizmo::DecomposeMatrixToComponents(&(*mat)[0][0], &pos.x, &rot.x, &scl.x);

	bool edited = false;

	if (ImGui::DragFloat3(("Position##" + tag).c_str(), &pos.x)) {
		edited = true;
	}
	if (ImGui::DragFloat3(("Rotation##" + tag).c_str(), &rot.x)) {
		edited = true;
	}
	if (ImGui::DragFloat3(("Scale##" + tag).c_str(), &scl.x)) {
		edited = true;
	}

	if (edited) {
		ImGuizmo::RecomposeMatrixFromComponents(&pos.x, &rot.x, &scl.x, &(*mat)[0][0]);
	}
	return edited;
}

int ExtraEditorGUI::TextSelected(ImGuiInputTextCallbackData* data)
{
	// TODO: Use re intererpept cast
	*((bool*)data->UserData) = true;
	return 0;
}

bool ExtraEditorGUI::ColourEdit3(std::string tag, glm::u8vec3& col, ImGuiColorEditFlags flags)
{
	glm::vec3 editColour = glm::vec3(col) / 255.0f;
	if (ImGui::ColorEdit3(("Colour##" + tag).c_str(), &editColour.x, flags)) {
		col = editColour * 255.0f;
		return true;
	}

	return false;
}

bool ExtraEditorGUI::SliderEnum(const std::vector<std::string>& names, int* value)
{
	std::string current = (*value >= 0 && *value < (int)names.size()) ? names.at(*value) : "Unknown";
	return ImGui::SliderInt("slider enum", value, 0, (int)names.size() - 1, current.c_str(), ImGuiSliderFlags_NoInput);
}

ExtraEditorGUI::ScopedIndent::ScopedIndent()
{
	ImGui::Indent();
}

ExtraEditorGUI::ScopedIndent::~ScopedIndent()
{
	ImGui::Unindent();
}

ExtraEditorGUI::ScopedDisable::ScopedDisable()
{
	ImGui::BeginDisabled();
}

ExtraEditorGUI::ScopedDisable::~ScopedDisable()
{
	ImGui::EndDisabled();
}

ExtraEditorGUI::ScopedBegin::ScopedBegin(std::string name, bool* p_open, ImGuiWindowFlags flags)
{
	open = ImGui::Begin(name.c_str(), p_open, flags);
}

ExtraEditorGUI::ScopedBegin::~ScopedBegin()
{
	ImGui::End();
}

ExtraEditorGUI::ScopedBegin::operator bool() const
{
	return open;
}
