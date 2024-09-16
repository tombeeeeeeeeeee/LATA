#include "Enemy.h"

#include "EditorGUI.h"
#include "Utilities.h"


void Enemy::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Enemy##" + tag).c_str())) {
		// TODO: Write
	}
}
