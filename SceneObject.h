#pragma once

#include "Transform.h"

#include "Maths.h"

class Model;
class Shader;

// TODO: Make version of the SceneObject that draws the same model in multiple spots, using instancing
class SceneObject
{
public:
	Transform transform;

	Model* model;
	Shader* shader;

	SceneObject();
	SceneObject(Model* _model, Shader* _shader, glm::vec3 _position = { 0.f, 0.f, 0.f }, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	void Draw() const;
};

