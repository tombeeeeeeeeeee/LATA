#pragma once
#include "Maths.h"
#include "CollisionPacket.h"

class RigidBody;
class SceneObject;

struct Collision
{
	RigidBody* rigidBody;
	SceneObject* sceneObject;
	glm::vec2 normal;
	int collisionMask;
	SceneObject* self;


};