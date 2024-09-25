#pragma once
#include <vector>

struct Enemy;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class SpawnManager
{
private:
	std::vector<unsigned long long> singleTimeSpawnLocations;
	std::vector<int> spawnPattern;

	float timeBetweenSpawns = 0.1f;
	float circleSpawnRadius = 0.0f;
	bool spawning = false;
	int currSpawnCount = 0;
	int indexInSpawnLoop = 0;

public:

	SpawnManager() {};
	SpawnManager(toml::table);

	int numToSpawn = 0;

	void GUI();
	void Serialise(unsigned long long guid);
};

