#pragma once
#include "Maths.h"

class Model;
class Shader;

class SceneObject
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;

	Model* model;
	Shader* shader;

	void Draw() const;
};

