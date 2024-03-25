#pragma once

#include "Maths.h"

class Transform
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;

	Transform();
	Transform(glm::vec3 _position, glm::vec3 _rotation, float _scale);

	glm::mat4 getMatrix() const;
};

