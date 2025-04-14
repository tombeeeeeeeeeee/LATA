#include "PixelCell.h"

#include "Utilities.h"
#include "ExtraEditorGUI.h"


void Pixels::Cell::GUI(int x, int y)
{
	std::string tag = Utilities::PointerToString(this);

	ImGui::BeginDisabled();
	int xy[2] = { x, y };
	ImGui::DragInt2("Global Position", &xy[0]);
	ImGui::EndDisabled();
	ExtraEditorGUI::ColourEdit3("Colour##" + tag, colour);
	ImGui::DragFloat2(("Velocity##" + tag).c_str(), &velocity.x);
	// TODO: Should be some better GUI option for this
	ImGui::InputScalar("Material ID", ImGuiDataType_U16, &materialID);
}
