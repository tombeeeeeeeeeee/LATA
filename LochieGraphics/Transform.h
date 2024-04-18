#pragma once

#include "Maths.h"

#include <vector>

class SceneObject;

class Transform
{
private:
	Transform* parent = nullptr;
	std::vector<Transform*> children = {};
	SceneObject* sceneObject;
	glm::quat quaternion;
	glm::vec3 euler;
public:
	Transform* getParent() const;
	SceneObject* getSceneObject() const;
	std::vector<Transform*> getChildren() const;
	bool isChildOf(Transform* transform) const;
	void setParent(Transform* newParent);
	void AddChild(Transform* newChild);
	void RemoveChild(Transform* oldChild);
	bool HasChildren();

	glm::vec3 position;

	float scale;

	glm::quat getRotation() const;
	glm::vec3 getEulerRotation() const;
	void setRotation(glm::quat _quat);
	void setEulerRotation(glm::vec3 _euler);

	Transform() = delete;
	Transform(SceneObject* _sceneObject, glm::vec3 _position = { 0.f, 0.f, 0.f }, glm::quat _quaternion = glm::quat({0.f, 0.f, 0.0f}), float _scale = 1.f);

	// TODO: Cache these, prob use dirty flags
	glm::mat4 getLocalMatrix() const;
	glm::mat4 getGlobalMatrix() const;

	void GUI();
};

