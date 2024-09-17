#include "SceneObject.h"

#include "ResourceManager.h"
#include "Animator.h"
#include "Collider.h"

#include "EditorGUI.h"

#include "Scene.h"

SceneObject::SceneObject(Scene* _scene, std::string _name) :
	scene(_scene),
	name(_name)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID] = Transform(this);
	scene->sceneObjects[GUID] = this;
	// TODO: Put a safetly check for if a guid that gets made is already on a sceneobject
}

SceneObject::SceneObject(Scene* _scene, glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	scene(_scene)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID] = Transform(this, _position, _rotation, _scale);
	scene->sceneObjects[GUID] = this;
}

SceneObject::~SceneObject()
{
	// TODO: Make sure we want to this this here
	ClearParts();
	scene->transforms.erase(GUID);
}

void SceneObject::Update(float delta)
{
}

void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	scene->transforms[GUID].GUI();

	if (parts & Parts::modelRenderer) { scene->renderers[GUID].GUI(); }
	if (parts & Parts::rigidBody) { scene->rigidBodies[GUID].GUI(); }
	if (parts & Parts::collider) { scene->colliders[GUID]->GUI(); }
	if (parts & Parts::enemy) { scene->enemies[GUID].GUI(); }
	if (parts & Parts::health) { scene->healths[GUID].GUI(); }
	

	if (parts & Parts::ecco)
	{
		if(scene->ecco)
			scene->ecco->GUI();
	}

	if (parts & Parts::sync)
	{
		if (scene->sync)
			scene->sync->GUI();
	}

	// TODO: Add animator parts;
	//if ((parts & Parts::animator)) {
	//	scene->animators[GUID].GUI();
	//}

	const char addPopup[] = "SceneObject Add Part";
	const char removePopup[] = "SceneObject Remove Part";

	if (ImGui::Button("Add Part")) {
		ImGui::OpenPopup(addPopup);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Part")) {
		ImGui::OpenPopup(removePopup);
	}

	if (ImGui::BeginPopup(addPopup)) {
		if (renderer() == nullptr) {
			if (ImGui::MenuItem("Model Renderer##Add part")) {
				setRenderer(new ModelRenderer());
			}
		}
		if (rigidbody() == nullptr) {
			if (ImGui::MenuItem("Rigid Body##Add part")) {
				setRigidBody(new RigidBody());
			}
		}
		if (ecco() == nullptr && scene->ecco->GUID == 0) {
			if (ImGui::MenuItem("Ecco##Add part")) {
				setEcco();
			}
		}
		if (sync() == nullptr && scene->sync->GUID == 0) {
			if (ImGui::MenuItem("Sync##Add part")) {
				setSync();
			}
		}
		if (health() == nullptr) {
			if (ImGui::MenuItem("Health##Add part")) {
				setHealth(new Health());
			}
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(removePopup)) {
		if (parts & Parts::modelRenderer) {
			if (ImGui::MenuItem("Model Renderer##Remove part")) {
				setRenderer(nullptr);
			}
		}
		if (parts & Parts::rigidBody) {
			if (ImGui::MenuItem("Rigid Body##Remove part")) {
				setRigidBody(nullptr);
			}
		}
		if (parts & Parts::ecco) {
			if (ImGui::MenuItem("Ecco##Remove part")) {
				setEcco(nullptr);
			}
		}
		if (parts & Parts::sync) {
			if (ImGui::MenuItem("Sync##Remove part")) {
				setSync(nullptr);
			}
		}
		ImGui::EndPopup();
	}
}

toml::table SceneObject::Serialise() const
{
	auto parent = transform()->getParent();
	unsigned long long parentGUID = 0;
	if (parent) {
		parentGUID = parent->getSceneObject()->GUID;
	}
	toml::array childrenGUIDs;
	for (auto child : transform()->getChildren())
	{
		childrenGUIDs.push_back(Serialisation::SaveAsUnsignedLongLong(child->getSceneObject()->GUID));
	}
	return toml::table{
		{ "name", name},
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "parts", parts},
		{ "parent", Serialisation::SaveAsUnsignedLongLong(parentGUID)},
		{ "children", childrenGUIDs }
	};
}

SceneObject::SceneObject(Scene* _scene, toml::table* table) :
	scene(_scene)
{
	name = Serialisation::LoadAsString((*table)["name"]);
	GUID = Serialisation::LoadAsUnsignedLongLong((*table)["guid"]);
	parts = Serialisation::LoadAsInt((*table)["parts"]);
	scene->transforms[GUID] = Transform(this);
	scene->sceneObjects[GUID] = this;
}

void SceneObject::setTransform(Transform* transform)
{
	scene->transforms[GUID] = (*transform);
}

Transform* SceneObject::transform() const
{
	return &(scene->transforms[GUID]);
}

#define setPart(part, container, enumValue)                     \
	if (part) {                                                 \
		parts |= enumValue;                                     \
 /*TODO: Ensure that this isn't leaking memory and is alright*/ \
		scene->container[GUID] = *part;                         \
	}                                                           \
	else {                                                      \
		parts &= ~enumValue;                                    \
		scene->container.erase(GUID);                           \
	}

#define getPart(container, enumValue)     \
	if (parts & enumValue) {              \
		return &(scene->container[GUID]); \
	}                                     \
	return nullptr

#define SetAndGetForPart(Type, container, enumValue, nameInSet, nameInGet) \
void SceneObject::set##nameInSet(Type* part)                               \
{                                                                          \
	setPart(part, container, enumValue);                                   \
}                                                                          \
Type * SceneObject::##nameInGet()                                          \
{                                                                          \
	getPart(container, enumValue);                                         \
}

SetAndGetForPart(ModelRenderer, renderers, Parts::modelRenderer, Renderer, renderer)
SetAndGetForPart(Animator, animators, Parts::animator, Animator, animator)
SetAndGetForPart(RigidBody, rigidBodies, Parts::rigidBody, RigidBody, rigidbody)
SetAndGetForPart(Health, healths, Parts::health, Health, health)
SetAndGetForPart(Enemy, enemies, Parts::enemy, Enemy, enemy)

void SceneObject::setCollider(Collider* collider)
{
	if (collider)
	{
		parts |= Parts::collider;
		scene->colliders[GUID] = collider;
	}
	else
	{
		parts &= ~Parts::collider;
		scene->colliders.erase(GUID);
	}
}

Collider* SceneObject::collider()
{
	if (parts & Parts::collider)
		return (scene->colliders[GUID]);
	return nullptr;
}

void SceneObject::setEcco(Ecco* ecco)
{
	if (ecco)
	{
		parts |= Parts::ecco;
		scene->ecco = ecco;
		scene->ecco->GUID = GUID;
	}
	else
	{
		parts &= ~Parts::ecco;
		if (scene->ecco && scene->ecco->GUID == GUID)
			scene->ecco->GUID = 0;
	}
}

void SceneObject::setEcco()
{
	parts |= Parts::ecco;	
	if(scene->ecco)
		scene->ecco->GUID = GUID;
}

Ecco* SceneObject::ecco() const
{
	if (parts & Parts::ecco)
		return scene->ecco;
	else return nullptr;
}

void SceneObject::setSync(Sync* sync)
{
	if (sync)
	{
		parts |= Parts::sync;
		scene->sync = sync;
		scene->sync->GUID = GUID;
	}
	else
	{
		parts &= ~Parts::sync;
		if (scene->sync && scene->sync->GUID == GUID)
			scene->sync->GUID = 0;
	}
}

void SceneObject::setSync()
{
	parts |= Parts::sync;
	if (scene->sync)
		scene->sync->GUID = GUID;
}

Sync* SceneObject::sync() const
{
	if (parts & Parts::sync)
		return scene->sync;
	else return nullptr;
}

void SceneObject::ClearParts()
{
	if (parts & Parts::modelRenderer) { scene->renderers.erase(GUID); }
	if (parts & Parts::animator) { scene->animators.erase(GUID); }
	if (parts & Parts::rigidBody) { scene->rigidBodies.erase(GUID); }
	if (parts & Parts::collider) { scene->colliders.erase(GUID); }
	if (parts & Parts::ecco) { scene->ecco->GUID = 0; }
	if (parts & Parts::sync) { scene->sync->GUID = 0; }
	if (parts & Parts::health) { scene->healths.erase(GUID); }
	if (parts & Parts::enemy) { scene->enemies.erase(GUID); }
	
	parts = Parts::transform;
}
