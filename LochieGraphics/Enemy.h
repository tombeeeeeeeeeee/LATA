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
	SceneObject* target = nullptr;
	glm::vec2 lastTargetPosition = { 0.0f, 0.0f };
	Node* currentNode = nullptr;
	std::vector<Node*> path = {};

	void GUI();
	toml::table Serialise(unsigned long long GUID);
};

