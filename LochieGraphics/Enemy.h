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

	int type = (int)EnemyType::spawnSpot;
	State* state = nullptr;

	glm::vec2 lastTargetPos = { 0.0f, 0.0f };
	glm::vec2 influenceThisFrame = { 0.0f, 0.0f };
	bool hasLOS = false;
	int nodeIndex = 0;
	std::vector<Node*> path = {};

	glm::vec2 boidVelocity = { 0.0f, 0.0f };

	void GUI();
	toml::table Serialise(unsigned long long GUID);
};

