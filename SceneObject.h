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

	SceneObject();
	SceneObject(Model* _model, Shader* _shader, glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	void Draw() const;
};

