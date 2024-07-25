#pragma once

#include "Maths.h"

#include <vector>

class SceneObject;

class Transform
{
private:
	Transform* parent = nullptr;
	std::vector<Transform*> children = {};
	glm::quat quaternion;
	glm::vec3 position;
	glm::vec3 scale;
	glm::mat4 globalMatrix;

public:
	Transform* getParent() const;
	std::vector<Transform*> getChildren() const;
	bool isChildOf(Transform* transform) const;
	void setParent(Transform* newParent);
	void AddChild(Transform* newChild);
	void RemoveChild(Transform* oldChild);
	bool HasChildren();

	glm::quat getRotation() const;
	glm::vec3 getEulerRotation() const;
	void setRotation(glm::quat _quat);
	void setEulerRotation(glm::vec3 _euler);

	glm::vec3 getScale();
	void setScale(float scale);
	void setScale(glm::vec3 scale);

	glm::vec3 getPosition();
	glm::vec3 getGlobalPosition();
	void setPosition(glm::vec3 pos);

	void UpdateGlobalMatrixCascading();

	Transform() = delete;
	Transform(glm::vec3 _position = { 0.f, 0.f, 0.f }, glm::quat _quaternion = glm::quat({0.f, 0.f, 0.0f}), float _scale = 1.f);

	glm::mat4 getLocalMatrix() const;
	glm::mat4 getGlobalMatrix() const;

	void GUI();
};

