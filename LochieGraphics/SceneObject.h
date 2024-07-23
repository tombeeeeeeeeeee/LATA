#pragma once
#include "SceneObject.h"

#include "Serialisation.h"

#include "Maths.h"

#include <vector>
#include <string>

class Shader;

// TODO: Make version of the SceneObject that draws the same model in multiple spots, using instancing
// TODO: Look into how instancing should be handled
class SceneObject
{
public:

	std::string name = "Unnamed Scene Object";

	SceneObject();
	SceneObject(glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	~SceneObject();

	SceneObject(const SceneObject& other) = delete;
	SceneObject& operator=(const SceneObject& other) = delete;

	void Update(float delta);
	void Draw(Shader* override = nullptr) const;
	void GUI();
};



