#pragma once
#include "Part.h"

#include "Maths.h"

class Transform : public Part
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;

	Transform();
	Transform(glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.f);

	glm::mat4 getMatrix() const;

	void GUI() override;
};

