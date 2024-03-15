#pragma once
#include "MeshRenderer.h"
#include "Transform.h"

class Entity
{
	Transform transform;
	MeshRenderer* meshRenderer;

	void Draw();
};

