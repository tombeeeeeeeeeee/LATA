#pragma once
#include "Maths.h"
#include <vector>

class RigidBody;
class SceneObject;
struct Collider;

struct Hit
{
	glm::vec2 normal;
	float distance;
	Collider* collider;
	SceneObject* sceneObject;
	std::vector<SceneObject*> otherCollisions;
};