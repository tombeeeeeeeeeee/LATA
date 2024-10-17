#include "SceneObject.h"

#include "ResourceManager.h"
#include "Animator.h"
#include "Collider.h"
#include "ExitElevator.h"
#include "Sync.h"
#include "Ecco.h"
#include "Scene.h"
#include "Paths.h"
#include "PrefabManager.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "LineRenderer.h"
#include "Serialisation.h"

#include <fstream>

SceneObject::SceneObject(Scene* _scene, std::string _name) :
	scene(_scene),
	name(_name)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID] = Transform(this);
	scene->sceneObjects[GUID] = this;
	// TODO: Put a safetly check for if a guid that gets made is already on a sceneobject
}

SceneObject::~SceneObject()
{
	// TODO: Make sure we want to this this here
	ClearParts();
	scene->transforms.erase(GUID);
}

#define AddPartGUI(getter, setter, type, label) \
if (getter() == nullptr) {                      \
if (ImGui::MenuItem(label)) {                   \
		setter(new type());                     \
	}                                           \
}

#define RemovePartGUI(partsType, setter, label) \
if (parts & Parts::partsType) {                 \
	if (ImGui::MenuItem(label)) {               \
		setter(nullptr);                        \
	}                                           \
}


void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	scene->transforms[GUID].GUI();

	if (prefabStatus == PrefabStatus::prefabOrigin) {
		// TODO: GUI for prefab origin
	}
	if (prefabStatus == PrefabStatus::prefabInstance) {

	}

	if (parts & Parts::modelRenderer) { scene->renderers[GUID].GUI(); }
	if (parts & Parts::rigidBody) { scene->rigidBodies[GUID].GUI(); }

	if (parts & Parts::collider) { 
		// Collapsing Header is not apart of the collider GUI as it can be apart of the rigidbody too
		if (ImGui::CollapsingHeader(("Collider##" + Utilities::PointerToString(this)).c_str())) {
			scene->colliders[GUID]->GUI();
		}
	}
	
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

	if (parts & Parts::exitElevator)
	{
		scene->exits[GUID].GUI(this);
	}
	// TODO: Add animator parts;
	if ((parts & Parts::animator)) {
		scene->animators[GUID].GUI();
	}

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
		AddPartGUI(renderer, setRenderer, ModelRenderer, "Model Renderer##Add part");
		AddPartGUI(rigidbody, setRigidBody, RigidBody, "Rigid Body##Add part");

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
		
		AddPartGUI(health, setHealth, Health, "Health##Add part");
		AddPartGUI(enemy, setEnemy, Enemy, "Enemy##Add part");
		AddPartGUI(exitElevator, setExitElevator, ExitElevator, "Exit Elevator##Add part");
		
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(removePopup)) {
		RemovePartGUI(modelRenderer, setRenderer, "Model Renderer##Remove part");
		RemovePartGUI(rigidBody, setRigidBody, "Rigid Body##Remove part");

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
		RemovePartGUI(exitElevator, setExitElevator, "Exit##Remove part");
		ImGui::EndPopup();
	}
}

void SceneObject::MenuGUI()
{
	std::string tag = Utilities::PointerToString(this);

	if (ImGui::MenuItem(("Delete##RightClick" + tag).c_str())) {
		scene->DeleteSceneObject(GUID);
	}
	if (ImGui::MenuItem((("Duplicate##RightClick") + tag).c_str())) {
		// TODO:
	}
	if (ImGui::MenuItem(("Save As Prefab##RightClick" + tag).c_str())) {
		SaveAsPrefab();
	}
	if (ImGui::MenuItem(("Replace with Prefab##RightClick" + tag).c_str())) {
		LoadFromPrefab(PrefabManager::loadedPrefabOriginals.at(PrefabManager::selectedPrefab));
	}
	if (ImGui::MenuItem(("Copy GUID##RightClick" + tag).c_str())) {
		ImGui::SetClipboardText(std::to_string(GUID).c_str());
	}
	ImGui::EndPopup();

}

void SceneObject::DebugDraw()
{
	if (parts & Parts::rigidBody)
	{
		scene->rigidBodies[GUID].DebugDraw(&scene->transforms[GUID]);
	}
	if (parts & Parts::collider)
	{
		scene->colliders[GUID]->DebugDraw(&scene->transforms[GUID]);
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
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID) },
		{ "parts", parts },
		{ "parent", Serialisation::SaveAsUnsignedLongLong(parentGUID)},
		{ "children", childrenGUIDs },
		{ "prefabStatus", (int)prefabStatus },
		{ "prefabBase", Serialisation::SaveAsUnsignedLongLong(prefabBase) }
	};
}

SceneObject::SceneObject(Scene* _scene, toml::table* table) :
	scene(_scene)
{
	name = Serialisation::LoadAsString((*table)["name"]);
	GUID = Serialisation::LoadAsUnsignedLongLong((*table)["guid"]);
	parts = Serialisation::LoadAsUnsignedInt((*table)["parts"]);
	scene->transforms[GUID] = Transform(this);
	scene->sceneObjects[GUID] = this;
	prefabStatus = (PrefabStatus)Serialisation::LoadAsInt((*table)["prefabStatus"]);
	prefabBase = Serialisation::LoadAsUnsignedLongLong((*table)["prefabBase"]);
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
SetAndGetForPart(ExitElevator, exits, Parts::exitElevator, ExitElevator, exitElevator)
SetAndGetForPart(SpawnManager, spawnManagers, Parts::spawnManager, SpawnManager, spawnManager)

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
	if (parts & Parts::modelRenderer) { scene->renderers.erase(GUID);     parts &= ~(Parts::modelRenderer); }
	if (parts & Parts::animator)      { scene->animators.erase(GUID);     parts &= ~(Parts::animator);}
	if (parts & Parts::rigidBody)     { scene->rigidBodies.erase(GUID);   parts &= ~(Parts::rigidBody);}
	if (parts & Parts::collider)      { scene->colliders.erase(GUID);     parts &= ~(Parts::collider);}
	if (parts & Parts::ecco)          { scene->ecco->GUID = 0;            parts &= ~(Parts::ecco);}
	if (parts & Parts::sync)          { scene->sync->GUID = 0;            parts &= ~(Parts::sync);}
	if (parts & Parts::health)        { scene->healths.erase(GUID);       parts &= ~(Parts::health);}
	if (parts & Parts::enemy)         { scene->enemies.erase(GUID);       parts &= ~(Parts::enemy);}
	if (parts & Parts::exitElevator)  { scene->exits.erase(GUID);         parts &= ~(Parts::exitElevator);}
	if (parts & Parts::spawnManager)  { scene->spawnManagers.erase(GUID); parts &= ~(Parts::spawnManager);}

	assert(parts == 0);
}

#define SaveAsPrefabPart(saveName, partsName, container)                      \
	if (Parts::##partsName & parts) {                                         \
		table.emplace(saveName, scene->##container.at(GUID).Serialise(GUID)); \
		safetyCheck &= ~Parts::##partsName;                                   \
	}                                                                         \


void SceneObject::SaveAsPrefab()
{
	auto safetyCheck = parts;
	std::ofstream file(Paths::prefabsSaveLocation + name + Paths::prefabExtension);

	toml::table table;

	table.emplace("sceneObject", Serialise());

	SaveAsPrefabPart("modelRenderer", modelRenderer, renderers);
	SaveAsPrefabPart("animator", animator, animators);
	SaveAsPrefabPart("rigidBody", rigidBody, rigidBodies);
	SaveAsPrefabPart("health", health, healths);
	SaveAsPrefabPart("enemy", enemy, enemies);
	SaveAsPrefabPart("exitElevator", exitElevator, exits);
	
	if (Parts::collider & parts) {
		table.emplace("collider", scene->colliders.at(GUID)->Serialise(GUID));
		safetyCheck &= ~Parts::collider;
	}
	if (Parts::ecco & parts) {
		table.emplace("ecco", scene->ecco->Serialise());
		safetyCheck &= ~Parts::ecco;
	}
	if (Parts::sync & parts) {
		table.emplace("sync", scene->sync->Serialise());
		safetyCheck &= ~Parts::sync;
	}

	// TODO: Probably don't need a whole assert here, could just print a error and continue on
	assert(safetyCheck == 0);

	// TODO: Give a warning incase the object has children as they won't be saved along with the prefab just yet

	file << table << '\n';

	file.close();

	prefabStatus = PrefabStatus::prefabOrigin;
	PrefabManager::loadedPrefabOriginals[GUID] = table;
}

#define LoadAsPrefabPart(saveName, partsName, setter, type) \
	if (intendedParts & Parts::##partsName) {               \
		/* TODO: likely leakig here, fix */                 \
		setter(new type(*table[saveName].as_table()));      \
	}                                                       \


void SceneObject::LoadFromPrefab(toml::table table)
{
	ClearParts();
	
	unsigned long long originalGUID = GUID;

	prefabStatus = PrefabStatus::prefabInstance;

	toml::table sceneObjectTable = *table["sceneObject"].as_table();

	unsigned long long intendedParts = Serialisation::LoadAsUnsignedInt(sceneObjectTable["parts"]);

	LoadAsPrefabPart("modelRenderer", modelRenderer, setRenderer, ModelRenderer);
	LoadAsPrefabPart("animator", animator, setAnimator, Animator);
	LoadAsPrefabPart("rigidBody", rigidBody, setRigidBody, RigidBody);
	LoadAsPrefabPart("health", health, setHealth, Health);
	LoadAsPrefabPart("enemy", enemy, setEnemy, Enemy);
	LoadAsPrefabPart("exitElevator", exitElevator, setExitElevator, ExitElevator);

	if (intendedParts & Parts::collider) {
		setCollider(Collider::Load(*table["collider"].as_table()));
	}
	if (intendedParts & Parts::ecco) {
		// TODO: This should load prefab data
		setEcco(scene->ecco);
	}
	if (intendedParts & Parts::sync) {
		// TODO: This should load prefab data
		setSync(scene->sync);
	}

	// TODO: Don't need a whole assert here
	assert(intendedParts == parts);
}
