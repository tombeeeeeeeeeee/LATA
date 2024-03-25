#pragma once

#include "Transform.h"
#include "ModelRenderer.h"

#include "Maths.h"

#include <vector>

class Model;
class Shader;

// TODO: Make version of the SceneObject that draws the same model in multiple spots, using instancing
class SceneObject
{
public:
	Transform transform;
	std::vector<Part*> parts;

	SceneObject();
	SceneObject(glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	~SceneObject();

	SceneObject(const SceneObject& other) = delete;
	SceneObject& operator=(const SceneObject& other) = delete;

	void Update(float delta);
	void Draw() const;

	void AddPart(Part* part);
};

