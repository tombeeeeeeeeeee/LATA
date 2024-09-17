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

	if (ImGui::DragFloat3(("Position" + tag).c_str(), &pos.x)) {
		edited = true;
	}
	if (ImGui::DragFloat3(("Rotation" + tag).c_str(), &rot.x)) {
		edited = true;
	}
	if (ImGui::DragFloat3(("Scale" + tag).c_str(), &scl.x)) {
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