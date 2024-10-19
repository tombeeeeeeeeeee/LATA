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
	int enemySpawnList = Serialisation::LoadAsInt(table["enemySpawnList"]);
	int enemySpawn = 0;
	int decimalSlot = 1;
	do 
	{
		enemySpawn = (enemySpawnList / decimalSlot) % 10;
		if (enemySpawn != 0)
		{
			spawnPattern.push_back(enemySpawn);
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
		else
		{
			if (triggeredOnce) triggeredOnce = false;
			spawning = false;
			triggerTag = "";
		}
	}
}

void SpawnManager::GUI()
{
	ImGui::DragInt("Number of enemies to spawn", &numToSpawn);
	ImGui::DragFloat("Time Between Spawns", &timeBetweenSpawns);
	ImGui::InputText("Trigger ID", &triggerTag);
}

toml::table SpawnManager::Serialise(unsigned long long guid)
{
	int enemySpawnList = 0;
	int decimalSlot = 1;
	for (int i = 0; i < spawnPattern.size(); i++, decimalSlot *= 10)
	{
		enemySpawnList += spawnPattern[i] * decimalSlot;
	}

	return toml::table{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{ "numToSpawn", numToSpawn },
		{ "timeBetweenSpawns", timeBetweenSpawns },
		{ "triggerTag", triggerTag },
		{ "enemySpawnList", enemySpawnList },
	};
}
