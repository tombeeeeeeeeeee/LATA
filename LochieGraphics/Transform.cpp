#include "Transform.h"

#include "SceneObject.h"
#include "Utilities.h"
#include "UserPreferences.h"

#include "EditorGUI.h"
#include "Serialisation.h"

#include "ImGuizmo.h"

#include <iostream>
#include <sstream>

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
	if (transform == parent && !transform) { return true; }
	for (Transform* search = parent; search != nullptr; search = search->parent)
	{
		if (search == transform) {
			return true;
		}
	}
	return false;
}

bool Transform::isDirectChildOf(Transform* transform) const
{
	if (transform == parent && !transform) { return true; }
	for (const auto i : transform->children)
	{
		if (i == this) {
			return true;
		}
	}
	return false;
}

bool Transform::isDirectParentOf(Transform* transform) const
{
	if (transform->parent == this) { return true; }
	return false;
}

void Transform::setParent(Transform* newParent)
{
	if (newParent) {
		if (newParent->isDirectChildOf(this)) {
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
	if (!children.empty() && std::find(children.begin(), children.end(), newChild) != children.end()) {
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
	if (children.empty()) {
		std::cout << "Error: 46828623948239490138\n";
		return;
	}
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

bool Transform::HasChildren() const
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

void Transform::setLocalMatrix(const glm::mat4& m)
{
	glm::vec3 newPos{}, newRot{}, newScale{};
	// TODO: Use own function
	ImGuizmo::DecomposeMatrixToComponents(&m[0][0], &newPos.x, &newRot.x, &newScale.x);
	position = newPos;
	scale = newScale;
	setEulerRotation(newRot); // setting the rotation updates the matrices
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

void Transform::setGlobalMatrix(const glm::mat4& m)
{
	if (parent)
	{
		/*
			globalMatrix = parent->getGlobalMatrix() * getLocalMatrix();
			gm = p.gm * lm
			inv(lm) * gm = p.gm
			inv(lm) = inv(gm) * p.gm
			lm = inv(inv(gm) * p.gm)
		*/
		setLocalMatrix(glm::inverse(glm::inverse(m) * parent->getGlobalMatrix()));
	}
	else
	{
		setLocalMatrix(m);
	}
}

void Transform::setRotation(glm::vec3 _euler)
{
	setEulerRotation(_euler);
}

void Transform::setEulerRotation(glm::vec3 _euler)
{
	euler = _euler;
	quaternion = glm::quat(glm::radians(_euler));
	UpdateGlobalMatrixCascading();
}

glm::vec3 Transform::getScale() const
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
	return glm::vec3(globalMatrix[3]);
}

glm::vec2 Transform::get2DGlobalPosition() const
{
	return { globalMatrix[3][0], globalMatrix[3][2] };
}

glm::vec2 Transform::get2DPosition() const
{
	return { position.x, position.z };
}

void Transform::setPosition(glm::vec3 pos)
{
	position = pos;
	UpdateGlobalMatrixCascading();
}

bool Transform::getStatic() const
{
	return isStatic;
}

void Transform::setStatic(bool staticness)
{
	isStatic = staticness;
	for (auto& i : children)
	{
		setStatic(staticness);
	}
}

glm::vec3 Transform::forward() const
{
	return glm::vec3(glm::normalize(globalMatrix[2]));
}

glm::vec3 Transform::right() const
{
	return -left();
}

glm::vec3 Transform::up() const
{
	return glm::vec3(glm::normalize(globalMatrix[1]));
}

glm::vec3 Transform::backward() const
{
	return -forward();
}

glm::vec3 Transform::down() const
{
	return -up();
}

glm::vec3 Transform::left() const
{
	return glm::vec3(glm::normalize(globalMatrix[0]));
}

void Transform::LookAt(glm::vec3 point, glm::vec3 up)
{
	glm::vec3 direction = glm::normalize(getGlobalPosition() - point);
	glm::quat rotAsQuat = glm::quatLookAt(direction, up);
	glm::vec3 rotAsEuler = glm::degrees(glm::eulerAngles(rotAsQuat));
	if (glm::isnan(rotAsEuler.x)) {
		return;
	}
	setEulerRotation(rotAsEuler);
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
	if (glm::isnan(globalMatrix[1][1])) {
		std::cout << "!!!\n";
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
	ImGui::OpenPopupOnItemClick(("Right Click##" + tag).c_str(), ImGuiPopupFlags_MouseButtonRight);
	if (ImGui::BeginPopup(("Right Click##" + tag).c_str())) {
		if (ImGui::MenuItem(("Reset##" + tag).c_str())) {
			setPosition({ 0.0f, 0.0f, 0.0f });
			setEulerRotation({ 0.0f, 0.0f, 0.0f });
			setScale({ 1.0f, 1.0f, 1.0f });
		}
		if (ImGui::MenuItem(("Copy##" + tag).c_str())) {
			std::stringstream clipboard;
			clipboard << Serialise(0);
			ImGui::SetClipboardText(clipboard.str().c_str());
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem(("Paste##" + tag).c_str())) {
			std::string clipboard = ImGui::GetClipboardText();
			try
			{
				toml::table data = toml::parse(clipboard);
				Load(data);
			}
			catch (const toml::parse_error& err)
			{
				std::cout << "Failed to paste transform info\n" << err << '\n';
			}
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem(("Show Advanced Info##" + tag).c_str(), nullptr, UserPreferences::advancedTransformInfo)) { 
			UserPreferences::advancedTransformInfo = !UserPreferences::advancedTransformInfo;
			UserPreferences::Save(); 
		}
		ImGui::EndPopup();
	}

	if (UserPreferences::advancedTransformInfo) {
		glm::vec3 l = left();
		glm::vec3 f = forward();
		glm::vec3 u = up();
		ImGui::Text("Direction Vectors");
		ImGui::BeginDisabled();
		ImGui::DragFloat3(("Left##" + tag).c_str(), &l.x);
		ImGui::DragFloat3(("Up##" + tag).c_str(), &u.x);
		ImGui::DragFloat3(("Forward##" + tag).c_str(), &f.x);
		ImGui::EndDisabled();

		ImGui::Text("Global Matrix");
		ImGui::BeginDisabled();
		ImGui::DragFloat4(("Left##globalMatrix" + tag).c_str(), &globalMatrix[0].x);
		ImGui::DragFloat4(("Up##globalMatrix" + tag).c_str(), &globalMatrix[1].x);
		ImGui::DragFloat4(("Forward##globalMatrix" + tag).c_str(), &globalMatrix[2].x);
		ImGui::DragFloat4(("Translation##globalMatrix" + tag).c_str(), &globalMatrix[3].x);
		ImGui::EndDisabled();
	}
	ImGui::Checkbox(("Static##" + tag).c_str(), &isStatic);

	if(ImGui::DragFloat3(("Position##transform" + tag).c_str(), &position[0], 0.1f))
	{
		UpdateGlobalMatrixCascading();
	}

	if (ImGui::DragFloat3(("Rotation##transform" + tag).c_str(), &euler[0], 0.1f)) {
		setEulerRotation(euler);
	}
	
	if (UserPreferences::advancedTransformInfo) {
		ImGui::BeginDisabled();
		if (ImGui::DragFloat4(("Quaternion##transform" + tag).c_str(), &quaternion[0], 0.1f)) {
			setRotation(quaternion);
		}
		ImGui::EndDisabled();
	}

	if (ImGui::DragFloat3(("Scale##transform" + tag).c_str(), &scale[0], 0.1f))
	{
		UpdateGlobalMatrixCascading();
	}
}

bool Transform::Gizmo(const glm::mat4& view, const glm::mat4& projection, unsigned int operation, unsigned int mode)
{
	glm::mat4 editMatrix;
	editMatrix = getGlobalMatrix();
	if (ImGuizmo::Manipulate(&view[0][0], &projection[0][0], (ImGuizmo::OPERATION)operation, (ImGuizmo::MODE)mode, &editMatrix[0][0])) {
		setGlobalMatrix(editMatrix);
		return true;
	}
	return false;
}

toml::table Transform::Serialise(unsigned long long GUID) const
{
	// NOTE: Parent / Children hierarchy are saved through the sceneobject
	return toml::table({
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "quaternion", Serialisation::SaveAsQuaternion(quaternion)},
		{ "euler", Serialisation::SaveAsVec3(euler)},
		{ "position", Serialisation::SaveAsVec3(position)},
		{ "scale", Serialisation::SaveAsVec3(scale)},
		{ "isStatic", isStatic},
		});
}

void Transform::Load(toml::table& table)
{
	quaternion = Serialisation::LoadAsQuaternion(table["quaternion"]);
	euler = Serialisation::LoadAsVec3(table["euler"]);
	position = Serialisation::LoadAsVec3(table["position"]);
	scale = Serialisation::LoadAsVec3(table["scale"]);
	isStatic = Serialisation::LoadAsBool(table["isStatic"], true);

	UpdateGlobalMatrixCascading();

}

Transform::Transform(toml::table table)
{
	Load(table);
}

Transform::~Transform()
{
	std::vector<Transform*> oldChildren = getChildren();
	for (auto i : oldChildren)
	{
		i->setParent(parent);
	}
	if (parent) {
		parent->RemoveChild(this);
	}
}
