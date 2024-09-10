#include "Transform.h"

#include "SceneObject.h"
#include "Utilities.h"

#include "EditorGUI.h"

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
	return euler;
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
	glm::vec3 newEuler = glm::degrees(glm::eulerAngles(_quat));
	glm::quat oldEulerAsQuat = glm::quat(glm::radians(euler));
	glm::vec3 oldEuler = glm::degrees(glm::eulerAngles(oldEulerAsQuat));
	glm::vec3 difEuler = newEuler - oldEuler;

	quaternion = _quat;

	euler += difEuler;
	
	UpdateGlobalMatrixCascading();
}

//TODO: Cache global matrix
glm::mat4 Transform::getGlobalMatrix() const
{
	return globalMatrix;
}

void Transform::setEulerRotation(glm::vec3 _euler)
{
	euler = _euler;
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
	UpdateGlobalMatrixCascading();

}

void Transform::setScale(glm::vec3 _scale)
{
	scale = _scale;
	UpdateGlobalMatrixCascading();

}

glm::vec3 Transform::getPosition() const
{
	return position;
}

glm::vec3 Transform::getGlobalPosition() const
{
	return { globalMatrix[3][0], globalMatrix[3][1], globalMatrix[3][2] };
}

void Transform::setPosition(glm::vec3 pos)
{
	position = pos;
	UpdateGlobalMatrixCascading();
}

glm::vec3 Transform::forward() const
{
	return glm::vec3(glm::normalize(globalMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
}

glm::vec3 Transform::right() const
{
	return glm::vec3(glm::normalize(globalMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
}

glm::vec3 Transform::up() const
{
	return glm::vec3(glm::normalize(globalMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
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

Transform::Transform()
{
	UpdateGlobalMatrixCascading();
}

void Transform::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (!ImGui::CollapsingHeader(("Transform##" + tag).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}


	if(ImGui::DragFloat3(("Position##transform" + tag).c_str(), &position[0], 0.1f))
	{
		UpdateGlobalMatrixCascading();
	}

	if (ImGui::DragFloat3(("Rotation##transform" + tag).c_str(), &euler[0], 0.1f)) {
		setEulerRotation(euler);
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

toml::table Transform::Serialise(unsigned long long GUID) const
{
	// TODO: Save children/parent
	return toml::table({
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "quaternion", Serialisation::SaveAsQuaternion(quaternion)},
		{ "euler", Serialisation::SaveAsVec3(euler)},
		{ "position", Serialisation::SaveAsVec3(position)},
		{ "scale", Serialisation::SaveAsVec3(scale)},
		});
}

Transform::Transform(toml::table table)
{
	quaternion = Serialisation::LoadAsQuaternion(table["quaternion"]);
	euler = Serialisation::LoadAsVec3(table["euler"]);
	position = Serialisation::LoadAsVec3(table["position"]);
	scale = Serialisation::LoadAsVec3(table["scale"]);

	UpdateGlobalMatrixCascading();
}

Transform::~Transform()
{
	if (parent) {
		auto temp = std::find(parent->children.begin(), parent->children.end(), this);
		if (temp != parent->children.end()) {
			parent->children.erase(temp);
		}
		else {
			std::cout << "Deleting Transform, parent was missing this in its children\n";
		}
	}
	for (auto i : children)
	{
		i->parent = parent;
	}
}
