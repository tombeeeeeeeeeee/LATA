#pragma once
#include "Maths.h"

#include <vector>
#include <string>

class SceneObject;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

enum class EnemyType
{
	spawnSpot = 1 << 1,
	explosive = 1 << 2,
	melee = 1 << 3,
	ranged = 1 << 4
};


struct Enemy
{
	Enemy() {};
	Enemy(toml::table table);
	Enemy(int _type, std::string _tag, int frame = 0) { type = _type; tag = _tag; frameForCheck = frame; };

	int type = (int)EnemyType::spawnSpot;


	glm::vec2 aim = { 0.0f, 0.0f };
	glm::vec2 influenceThisFrame = { 0.0f, 0.0f };
	glm::vec2 boidVelocity = { 0.0f, 0.0f };

	int frameForCheck = 0;
	bool hasLOS = false;
	glm::vec2 target = {};

	float timeInAbility = 0.0f;
	bool inAbility = false;
	bool fleeing = false;

	std::string tag = "";

	void GUI();
	toml::table Serialise(unsigned long long GUID);
};

