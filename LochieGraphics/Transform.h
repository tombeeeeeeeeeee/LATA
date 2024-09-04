#pragma once

#include "Maths.h"

#include "Serialisation.h"

#include <vector>

class SceneObject;

class Transform
{
private:
	Transform* parent = nullptr;
	std::vector<Transform*> children = {};
	glm::quat quaternion = glm::identity<glm::quat>();
	glm::vec3 euler = glm::zero<glm::vec3>();
	glm::vec3 position = glm::zero<glm::vec3>();
	glm::vec3 scale = {1,1,1};
	glm::mat4 globalMatrix = glm::identity<glm::mat4>();

public:
	SceneObject* getSceneObject() const;
	//TODO: Add pointless pointer to LGP3
	SceneObject* so = nullptr;

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

	glm::vec3 getPosition() const;
	glm::vec3 getGlobalPosition() const;
	void setPosition(glm::vec3 pos);

	glm::vec3 forward() const;
	glm::vec3 right() const;
	glm::vec3 up() const;

	void UpdateGlobalMatrixCascading();

	Transform() { UpdateGlobalMatrixCascading(); }

	Transform(SceneObject* so,
		glm::vec3 _position = { 0.f, 0.f, 0.f }, 
		glm::quat _quaternion = glm::quat({0.f, 0.f, 0.0f}), 
		float _scale = 1.f,
		Transform* parent = nullptr
	);

	glm::mat4 getLocalMatrix() const;
	glm::mat4 getGlobalMatrix() const;

	void GUI();

	toml::table Serialise(unsigned long long GUID) const;
	Transform(toml::table table);

	// Transform destructor should make childrens parent this objects parent, and potentally remove this object parents children reference to this
};

