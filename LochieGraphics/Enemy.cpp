#include "Enemy.h"

#include "Utilities.h"

#include "EditorGUI.h"


void Enemy::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Enemy##" + tag).c_str())) {
		// TODO: Write
	}
}
