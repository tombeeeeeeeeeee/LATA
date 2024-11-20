#include "InvisibleWall.h"
#include "Serialisation.h"
#include "imgui.h"
#include "Utilities.h"

void ShadowWall::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Shadow Wall##" + tag).c_str()))
	{
		ImGui::Indent();

		ImGui::Checkbox("Uses the mesh of the renderer its attached to", &hasLocalMesh);

		ImGui::Unindent();
	}
}

ShadowWall::ShadowWall(toml::table table)
{
	hasLocalMesh = Serialisation::LoadAsBool(table["hasLocalMesh"]);
}

toml::table ShadowWall::Serialise(unsigned long long guid)
{
	return toml::table{ 
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{"hasLocalMesh", hasLocalMesh},
	};
}
