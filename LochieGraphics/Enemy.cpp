#include "Enemy.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

Enemy::Enemy(toml::table table)
{
	type = (EnemyType)Serialisation::LoadAsInt(table["type"]);
}

void Enemy::GUI()
{
	std::string tag = Utilities::PointerToString(this);

	if (ImGui::CollapsingHeader(("Enemy##" + tag).c_str())) {
		bool explosive = (1 << (int)EnemyType::explosive & 1 << (int)type);
		bool melee = (1 << (int)EnemyType::melee & 1 << (int)type);
		bool ranged = (1 << (int)EnemyType::ranged & 1 << (int)type);

		if (ImGui::Checkbox("Explosive type:", &explosive))
			type = EnemyType::explosive;
		if (ImGui::Checkbox("Melee type:", &melee))
			type = EnemyType::melee;
		if (ImGui::Checkbox("Ranged type:", &ranged))
			type = EnemyType::ranged;
	}
}

toml::table Enemy::Serialise()
{
	return toml::table
	{
		{"type", (int)type}
	};
}
