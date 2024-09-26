#include "Enemy.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

Enemy::Enemy(toml::table table)
{
	type = Serialisation::LoadAsInt(table["type"]);
}

void Enemy::GUI()
{
	std::string tag = Utilities::PointerToString(this);

	if (ImGui::CollapsingHeader(("Enemy##" + tag).c_str())) {
		bool spawnSpot = ((int)EnemyType::spawnSpot & type);
		bool explosive = ((int)EnemyType::explosive & type);
		bool melee = ((int)EnemyType::melee & type);
		bool ranged = ((int)EnemyType::ranged & type);

		if (ImGui::Checkbox("Spawn Spot: ", &spawnSpot))
		{
			if(spawnSpot) type |= (int)EnemyType::spawnSpot;
			else type &= ~(int)EnemyType::spawnSpot;
		}

		if (ImGui::Checkbox("Explosive type:", &explosive))
		{
			if (type & (int)EnemyType::spawnSpot)
				type = (int)EnemyType::spawnSpot;
			else
				type = 0;

			type |= (int)EnemyType::explosive;
		}
			
		if (ImGui::Checkbox("Melee type:", &melee))
		{
			if (type & (int)EnemyType::spawnSpot)
				type = (int)EnemyType::spawnSpot;
			else
				type = 0;

			type |= (int)EnemyType::melee;
		}
		if (ImGui::Checkbox("Ranged type:", &ranged))
		{
			if (type & (int)EnemyType::spawnSpot)
				type = (int)EnemyType::spawnSpot;
			else
				type = 0;

			type |= (int)EnemyType::ranged;
		}
	}
}

toml::table Enemy::Serialise(unsigned long long GUID)
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{"type", type},
	};
}
