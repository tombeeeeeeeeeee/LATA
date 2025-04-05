#include "PixelCell.h"

#include "Utilities.h"
#include "ExtraEditorGUI.h"


void Pixels::Cell::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ExtraEditorGUI::ColourEdit3("Colour##" + tag, colour);
	ImGui::DragFloat2(("Velocity##" + tag).c_str(), &velocity.x);
	// TODO: Should be some better GUI option for this
	ImGui::InputScalar("Material ID", ImGuiDataType_U16, &materialID);
}
