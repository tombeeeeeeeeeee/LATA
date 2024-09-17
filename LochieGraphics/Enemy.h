#pragma once
#include "Maths.h"
#include <vector>

struct State;
class SceneObject;
struct Node;

enum class EnemyType
{
	basic = 1,
	smart = 2,
	other = 3,
};


struct Enemy
{
	EnemyType type = EnemyType::basic;
	State* state;
	SceneObject* target;
	glm::vec3 lastTargetPosition;
	Node* currentNode;
	std::vector<Node*> path;

	void GUI();
};

