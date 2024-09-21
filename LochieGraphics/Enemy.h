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
	explosive,
	melee,
	ranged
};


struct Enemy
{
	Enemy() {};
	Enemy(toml::table table);

	EnemyType type = EnemyType::melee;
	State* state;
	SceneObject* target;
	glm::vec3 lastTargetPosition;
	Node* currentNode;
	std::vector<Node*> path;

	void GUI();
	toml::table Serialise();
};

