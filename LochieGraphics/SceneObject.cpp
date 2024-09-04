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

}

void SceneObject::Update(float delta)
{
}

void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	scene->transforms[GUID].GUI();

	if (parts & Parts::modelRenderer) {
		scene->renderers[GUID].GUI();
	}

	if (parts & Parts::rigidBody) {
		scene->rigidBodies[GUID].GUI();
	}

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

	//TODO Add animator parts;
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
	return toml::table{
		{"name", name},
		{"guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{"parts", parts}
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

Transform* SceneObject::transform()
{
	return &(scene->transforms[GUID]);
}

void SceneObject::setRenderer(ModelRenderer* renderer)
{
	if (renderer)
	{
		parts |= Parts::modelRenderer;
		scene->renderers[GUID] = *renderer;
		scene->renderers[GUID].sceneObject = this;
	}
	else
	{
		parts &= ~Parts::modelRenderer;
		scene->renderers.erase(GUID);
	}
}

ModelRenderer* SceneObject::renderer()
{
	if (parts & Parts::modelRenderer)
		return &(scene->renderers[GUID]);
	return nullptr;
}

void SceneObject::setAnimator(Animator* animator)
{
	if (animator)
	{
		parts |= Parts::animator;
		scene->animators[GUID] = *animator;
	}
	else
	{
		parts &= ~Parts::animator;
		scene->animators.erase(GUID);
	}
}

Animator* SceneObject::animator()
{
	if (parts & Parts::animator)
		return &(scene->animators[GUID]);
	return nullptr;
}

void SceneObject::setRigidBody(RigidBody* rb)
{
	if (rb)
	{
		parts |= Parts::rigidBody;
		scene->rigidBodies[GUID] = *rb;
	}
	else
	{
		parts &= ~Parts::rigidBody;
		scene->rigidBodies.erase(GUID);
	}
}

RigidBody* SceneObject::rigidbody()
{
	if (parts & Parts::rigidBody)
		return &(scene->rigidBodies[GUID]);
	return nullptr;
}

void SceneObject::setCollider(Collider* collider)
{
	if (collider)
	{
		parts |= Parts::collider;
		scene->colliders[GUID] = *collider;
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
		return &(scene->colliders[GUID]);
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

void SceneObject::setHealth(Health* health)
{
	if (health)
	{
		parts |= Parts::health;
		scene->healths[GUID] = *health;
	}
	else
	{
		parts &= ~Parts::health;
		scene->healths.erase(GUID);
	}
}

Health* SceneObject::health()
{
	if (parts & Parts::health)
		return &(scene->healths[GUID]);
	return nullptr;
}

void SceneObject::setEnemy(Enemy* enemy)
{
	if (enemy)
	{
		parts |= Parts::enemy;
		scene->enemies[GUID] = *enemy;
	}
	else
	{
		parts &= ~Parts::enemy;
		scene->enemies.erase(GUID);
	}
}

Enemy* SceneObject::enemy()
{
	if (parts & Parts::enemy)
		return &(scene->enemies[GUID]);
	return nullptr;
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
