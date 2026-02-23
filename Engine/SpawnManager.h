#pragma once
#include <vector>
#include <string>

struct Enemy;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class SpawnManager
{
public:

	std::vector<int> spawnPattern;

	float timeBetweenSpawns = 0.15f;
	bool spawning = false;
	int currSpawnCount = 0;
	
	bool triggeredOnce = false;
	bool canBeDisabled = false;

	std::string triggerTag;
	std::string enemyTriggerTag;
	SpawnManager() {};
	void Load(toml::table table);
	SpawnManager(toml::table);

	void TriggerCall(std::string tag, bool toggle);

	int indexInSpawnLoop = 0;
	int numToSpawn = 0;

	float timeSinceLastSpawn = 0.0f;

	void GUI();
	toml::table Serialise(unsigned long long guid);
};

