#include "Transform.h"

#include "SceneObject.h"
#include "Utilities.h"

#include "imgui.h"

#include <string>
#include <iostream>

Transform* Transform::getParent() const
{
	return parent;
}

SceneObject* Transform::getSceneObject() const
{
	return sceneObject;
}

std::vector<Transform*> Transform::getChildren() const
{
	return children;
}

bool Transform::isChildOf(Transform* transform) const
{
	for (Transform* search = parent; search != nullptr; search = search->parent)
	{
		if (search == transform) {
			return true;
		}
	}
	return false;
}

void Transform::setParent(Transform* newParent)
{
	if (newParent->isChildOf(this)) {
		std::cout << "Error, Unemplemented Behaviour, trying to set the parent of a transform as its child\n";
		return;
	}
	if (parent) {
		parent->RemoveChild(this);
	}
	parent = newParent;
	if (newParent) {
		newParent->AddChild(this);
	}
}

void Transform::AddChild(Transform* newChild)
{
	if (std::find(children.begin(), children.end(), newChild) != children.end()) {
		// Child already exists as a child
		if (newChild->parent != this) {
			std::cout << "Error: Trying to add a child that was already a child but had a different parent!\n";
		}
		return;
	}
	children.push_back(newChild);
	newChild->parent = this;
}

void Transform::RemoveChild(Transform* oldChild)
{
	auto childIterator = std::find(children.begin(), children.end(), oldChild);
	if (childIterator == children.end()) {
		// Child not found
		std::cout << "Error: Trying to remove a transform as a child that wasn't a child\n";
		return;
	}

	(*childIterator)->parent = nullptr;
	children.erase(childIterator);
}

bool Transform::HasChildren()
{
	return !children.empty();
}

Transform::Transform(SceneObject* _sceneObject, glm::vec3 _position, glm::quat _quaternion, float _scale) :
	sceneObject(_sceneObject),
	parent(nullptr),
	position(_position),
	quaternion(_quaternion),
	euler(glm::degrees(glm::eulerAngles(_quaternion))),
	scale(_scale)
{
}

glm::quat Transform::getRotation() const
{
	return quaternion;
}

glm::vec3 Transform::getEulerRotation() const
{
	return euler;
}

glm::mat4 Transform::getLocalMatrix() const
{
	glm::mat4 matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, position);
	matrix = glm::scale(matrix, glm::vec3(scale));
	matrix = matrix * glm::mat4_cast(quaternion);
	//matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	return matrix;
}

void Transform::setRotation(glm::quat _quat)
{
	quaternion = _quat;
	euler = glm::degrees(glm::eulerAngles(quaternion));
}

glm::mat4 Transform::getGlobalMatrix() const
{
	if (parent) {
		return parent->getGlobalMatrix() * getLocalMatrix();
	}
	return getLocalMatrix();
}

void Transform::setEulerRotation(glm::vec3 _euler)
{
	euler = _euler;
	quaternion = glm::quat(glm::radians(euler));
}

void Transform::GUI()
{
	std::string tag = PointerToString(this);

	ImGui::DragFloat3(("Position##transform" + tag).c_str(), &position[0], 0.1f);

	if (ImGui::DragFloat3(("Rotation##transform" + tag).c_str(), &euler[0], 0.1f)) {
		setEulerRotation(euler);
	}
	ImGui::BeginDisabled();
	if (ImGui::DragFloat4(("Quaternion##transform" + tag).c_str(), &quaternion[0], 0.1f)) {
		setRotation(quaternion);
		//setEulerRotation(euler);
	}
	ImGui::EndDisabled();

	ImGui::DragFloat(("Scale##transform" + tag).c_str(), &scale, 0.1f);
}