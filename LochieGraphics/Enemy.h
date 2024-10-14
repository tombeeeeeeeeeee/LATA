#pragma once
#include "Maths.h"

#include <vector>

struct State;
class SceneObject;
struct Node;
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
	Enemy(int _type) { type = _type; };

	int type = (int)EnemyType::spawnSpot;

	glm::vec2 lastTargetPos = { 0.0f, 0.0f };
	glm::vec2 influenceThisFrame = { 0.0f, 0.0f };
	bool hasLOS = false;

	glm::vec2 boidVelocity = { 0.0f, 0.0f };

	float timeInAbility = 0.0f;
	bool inAbility = false;
	bool fleeing = false;

	void GUI();
	toml::table Serialise(unsigned long long GUID);
};

