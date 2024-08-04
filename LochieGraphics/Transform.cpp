#include "Transform.h"

#include "SceneObject.h"
#include "Utilities.h"

#include "imgui.h"

#include <string>
#include <iostream>

SceneObject* Transform::getSceneObject() const
{
	return so;
}

Transform* Transform::getParent() const
{
	return parent;
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
	if (newParent) {
		if (newParent->isChildOf(this)) {
			std::cout << "Error, Unemplemented Behaviour, trying to set the parent of a transform as its child\n";
			return;
		}
		else if(newParent == this)
		{
			std::cout << "Error, Unemplemented Behaviour, trying to set a transform as its own child\n";
			return;
		}
	}
	if (parent) {
		parent->RemoveChild(this);
	}
	if (!newParent) {
		parent = nullptr;
		return;
	}
	newParent->AddChild(this);
	UpdateGlobalMatrixCascading();
}

void Transform::AddChild(Transform* newChild)
{
	if (newChild == this)
	{
		std::cout << "Error, Unemplemented Behaviour, trying to set a transform as its own child\n";
		return;
	}
	if (std::find(children.begin(), children.end(), newChild) != children.end()) {
		// Child already exists as a child
		if (newChild->parent != this) {
			std::cout << "Error: Trying to add a child that was already a child but had a different parent!\n";
		}
		return;
	}
	children.push_back(newChild);
	newChild->parent = this;
	newChild->UpdateGlobalMatrixCascading();
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
	(*childIterator)->UpdateGlobalMatrixCascading();
	children.erase(childIterator);
}

bool Transform::HasChildren()
{
	return !children.empty();
}

Transform::Transform(SceneObject* _so, glm::vec3 _position, glm::quat _quaternion, float _scale, Transform* _parent) :
	so(_so),
	position(_position),
	quaternion(_quaternion),
	scale({ _scale, _scale, _scale }),
	parent(_parent)
{
	UpdateGlobalMatrixCascading();
}

glm::quat Transform::getRotation() const
{
	return quaternion;
}

glm::vec3 Transform::getEulerRotation() const
{
	glm::vec3 eulerAngles = glm::eulerAngles(quaternion);
	return eulerAngles * 180.0f / glm::pi<float>();
}

glm::mat4 Transform::getLocalMatrix() const
{
	glm::mat4 identity = glm::identity<glm::mat4>();
	glm::mat4 translation = glm::translate(identity, position);
	glm::mat4 scaling = glm::scale(identity, scale);
	glm::mat4 rot = glm::mat4_cast(quaternion);
	//matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//matrix =  glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	return translation * rot * scaling;
}

void Transform::setRotation(glm::quat _quat)
{
	quaternion = _quat;
	UpdateGlobalMatrixCascading();
}

//TODO: Cache global matrix
glm::mat4 Transform::getGlobalMatrix() const
{
	return globalMatrix;
}

void Transform::setEulerRotation(glm::vec3 _euler)
{
	quaternion = glm::quat(glm::radians(_euler));
	UpdateGlobalMatrixCascading();
}

glm::vec3 Transform::getScale()
{
	return scale;
}

void Transform::setScale(float _scale)
{
	scale = { _scale, _scale, _scale };
}

void Transform::setScale(glm::vec3 _scale)
{
	scale = _scale;
}

glm::vec3 Transform::getPosition()
{
	return position;
}

glm::vec3 Transform::getGlobalPosition()
{
	return { globalMatrix[3][0], globalMatrix[3][1], globalMatrix[3][2] };
}

void Transform::setPosition(glm::vec3 pos)
{
	position = pos;
}

void Transform::UpdateGlobalMatrixCascading()
{
	if (parent)
	{
		globalMatrix = parent->getGlobalMatrix() * getLocalMatrix();
	}
	else
	{
		globalMatrix = getLocalMatrix();
	}

	for(auto i = children.begin(); i != children.end(); i++)
	{
		(*i)->UpdateGlobalMatrixCascading();
	}
}

void Transform::GUI()
{
	std::string tag = Utilities::PointerToString(this);

	glm::vec3 euler = getEulerRotation();

	if(ImGui::DragFloat3(("Position##transform" + tag).c_str(), &position[0], 0.1f))
	{
		UpdateGlobalMatrixCascading();
	}

	if (ImGui::DragFloat3(("Rotation##transform" + tag).c_str(), &euler[0], 0.1f)) {
		euler -= getEulerRotation();

		glm::vec3 rotationEuler = glm::vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));

		glm::quat quatZ = glm::angleAxis(rotationEuler.z, glm::vec3(0, 0, 1));
		glm::quat quatY = glm::angleAxis(rotationEuler.y, glm::vec3(0, 1, 0));
		glm::quat quatX = glm::angleAxis(rotationEuler.x, glm::vec3(1, 0, 0));

		setRotation(glm::normalize(quatX * quatY * quatZ) * getRotation());
	}
	ImGui::BeginDisabled();
	if (ImGui::DragFloat4(("Quaternion##transform" + tag).c_str(), &quaternion[0], 0.1f)) {
		setRotation(quaternion);
	}
	ImGui::EndDisabled();

	if (ImGui::DragFloat3(("Scale##transform" + tag).c_str(), &scale[0], 0.1f))
	{
		UpdateGlobalMatrixCascading();
	}
}