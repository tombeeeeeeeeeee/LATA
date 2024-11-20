#pragma once

#include "Maths.h"

#include <vector>

class SceneObject;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

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
	bool isDirectChildOf(Transform* transform) const;
	bool isDirectParentOf(Transform* transform) const;
	void setParent(Transform* newParent);
	void AddChild(Transform* newChild);
	void RemoveChild(Transform* oldChild);
	bool HasChildren() const;

	glm::quat getRotation() const;
	glm::vec3 getEulerRotation() const;
	void setRotation(glm::quat _quat);
	// Only made this overload because glm can cast vec3 to quat
	void setRotation(glm::vec3 _euler);
	void setEulerRotation(glm::vec3 _euler);

	glm::vec3 getScale() const;
	void setScale(float scale);
	void setScale(glm::vec3 scale);

	glm::vec3 getPosition() const;
	glm::vec3 getGlobalPosition() const;
	glm::vec2 get2DGlobalPosition() const;
	glm::vec2 get2DPosition() const;
	void setPosition(glm::vec3 pos);

	glm::vec3 forward() const;
	glm::vec3 right() const;
	glm::vec3 up() const;
	glm::vec3 backward() const;
	glm::vec3 down() const;
	glm::vec3 left() const;

	void LookAt(glm::vec3 point, glm::vec3 up);

	void UpdateGlobalMatrixCascading();

	Transform();

	Transform(SceneObject* so,
		glm::vec3 _position = { 0.f, 0.f, 0.f }, 
		glm::quat _quaternion = glm::quat({0.f, 0.f, 0.0f}), 
		float _scale = 1.f,
		Transform* parent = nullptr
	);

	glm::mat4 getLocalMatrix() const;
	void setLocalMatrix(const glm::mat4& m);
	glm::mat4 getGlobalMatrix() const;
	// Changes the local matrix to match the given global matrix
	void setGlobalMatrix(const glm::mat4& m);

	void GUI();
	bool Gizmo(const glm::mat4& view, const glm::mat4& projection, unsigned int operation, unsigned int mode);

	toml::table Serialise(unsigned long long GUID) const;
	void Load(toml::table& table);
	Transform(toml::table table);

	~Transform();

	// TODO:
	Transform(const Transform& other) = delete;
	Transform& operator=(const Transform& other) = delete;
};

