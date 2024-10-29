#include "SpawnManager.h"
#include "imgui.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"
#include "TriggerSystem.h"

SpawnManager::SpawnManager(toml::table table)
{
	numToSpawn = Serialisation::LoadAsInt(table["numToSpawn"]);
	timeBetweenSpawns = Serialisation::LoadAsFloat(table["timeBetweenSpawns"]);
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
	canBeDisabled = Serialisation::LoadAsBool(table["canBeDisabled"]);
	enemyTriggerTag = Serialisation::LoadAsString(table["enemyTriggerTag"]);

	int enemySpawnList = Serialisation::LoadAsInt(table["enemySpawnList"]);
	int enemySpawn = 0;
	int decimalSlot = 1;
	do 
	{
		enemySpawn = (enemySpawnList / decimalSlot) % 10;
		if (enemySpawn != 0)
		{
			spawnPattern.push_back(enemySpawn - 1);
		}
		decimalSlot *= 10;
	} while (enemySpawn != 0);
}

void SpawnManager::TriggerCall(std::string tag, bool toggle)
{
	if (tag == triggerTag)
	{
		if (toggle)
		{
			if (!triggeredOnce)
			{
				spawning = true;
				timeSinceLastSpawn = FLT_MAX;
			}
		}

		//disables the spawner
		else if(canBeDisabled)
		{
			if (triggeredOnce) triggeredOnce = false;
			spawning = false;
			triggerTag = "";
		}
	}
}

void SpawnManager::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Spawn Manager##" + tag).c_str()))
	{
		ImGui::Indent();
		ImGui::DragInt("Number of enemies to spawn", &numToSpawn);
		ImGui::DragFloat("Time Between Spawns", &timeBetweenSpawns);
		ImGui::Checkbox("Can Be Disabled", &canBeDisabled);
		ImGui::InputText("Trigger ID", &triggerTag);
		ImGui::InputText("Enemy Trigger ID", &enemyTriggerTag);

		const char* enemyTypes[] = { "Explosive", "Melee", "Ranged" };



		ImGui::Text("Spawn Pattern: ");
		ImGui::Indent();
		if(ImGui::BeginTable("Spawn Order", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Enemy Type");

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Remove");
			for (int i = 0; i < spawnPattern.size(); i++)
			{
				ImGui::TableNextRow();

				const char* currType = enemyTypes[spawnPattern[i]];
				ImGui::TableSetColumnIndex(0);
				ImGui::PushItemWidth(180);
				if (ImGui::BeginCombo(("##enemyInPattern" + std::to_string(i)).c_str(), currType))
				{
					for (int j = 0; j < 3; j++)
					{
						bool isSelected = j == spawnPattern[i];
						if (ImGui::Selectable(enemyTypes[j], isSelected))
						{
							spawnPattern[i] = j;
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Button(("X##" + std::to_string(i)).c_str()))
				{
					std::vector<int> temp = {};
					for (int j = 0; j < spawnPattern.size(); j++)
					{
						if (j == i) continue;
						temp.push_back(spawnPattern[j]);
					}
					spawnPattern = temp;
				}
			}
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);
			if (ImGui::Button("Remove Enemy"))
			{
				spawnPattern.pop_back();
			}
			ImGui::TableSetColumnIndex(0);
			if (ImGui::Button("Add Enemy"))
			{
				spawnPattern.push_back(0);
			}

			ImGui::EndTable();
			ImGui::Unindent();
		}
		ImGui::Unindent();
	}
}

toml::table SpawnManager::Serialise(unsigned long long guid)
{
	int enemySpawnList = 0;
	int decimalSlot = 1;
	for (int i = 0; i < spawnPattern.size(); i++, decimalSlot *= 10)
	{
		enemySpawnList += (spawnPattern[i] + 1) * decimalSlot;
	}

	return toml::table{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{ "numToSpawn", numToSpawn },
		{ "timeBetweenSpawns", timeBetweenSpawns },
		{ "triggerTag", triggerTag },
		{ "canBeDisabled", canBeDisabled },
		{ "enemyTriggerTag", enemyTriggerTag },
		{ "enemySpawnList", enemySpawnList },
	};
}
