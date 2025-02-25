#include "SceneObject.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Paths.h"
#include "PrefabManager.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "LineRenderer.h"
#include "Serialisation.h"

#include <fstream>
#include <array>

#define SavePart(saveName, partsName, container)                              \
	if (Parts::##partsName & parts) {                                         \
		table.emplace(saveName, scene->##container.at(GUID).Serialise(GUID)); \
		safetyCheck &= ~Parts::##partsName;                                   \
	}

#define LoadPart(saveName, partsName, setter, type)         \
	if (intendedParts & Parts::##partsName) {               \
		/* TODO: likely leaking here, fix */                \
		setter(new type(*table[saveName].as_table()));      \
	}

#define AddPartGUI(getter, setter, constructor, label) \
if (getter() == nullptr) {                             \
if (ImGui::MenuItem(label)) {                          \
		setter(new constructor);                       \
	}                                                  \
}

#define RemovePartGUI(partsType, setter, label) \
if (parts & Parts::partsType) {                 \
	if (ImGui::MenuItem(label)) {               \
		setter(nullptr);                        \
	}                                           \
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

#define getPart(container, enumValue)        \
	if (parts & enumValue) {                 \
		return &(scene->container.at(GUID)); \
	}                                        \
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

SceneObject::SceneObject(Scene* _scene, std::string _name) :
	scene(_scene),
	name(_name)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID].so = this;
	scene->sceneObjects[GUID] = this;
	// TODO: Put a safetly check for if a guid that gets made is already on a sceneobject
}

SceneObject::~SceneObject()
{
	ClearParts();
	scene->transforms.erase(GUID);
}

void SceneObject::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::InputText(("Name##" + tag).c_str(), &name);

	if (prefabStatus == PrefabStatus::origin) {
		if (ImGui::Button(("Save Prefab##" + tag).c_str())) {
			SaveAsPrefab();
		}
		ImGui::SameLine();
		if (ImGui::Button(("Refresh Prefab Instances##" + tag).c_str())) {
			PrefabManager::RefreshPrefabInstancesOf(GUID);
		}
	}
	if (prefabStatus == PrefabStatus::instance) {
		if (ImGui::Button(("Refresh This Instance##" + tag).c_str())) {
			LoadFromPrefab(PrefabManager::loadedPrefabOriginals.at(prefabBase));
		}
		// TODO: Button to 'overwrite' the prefab, like apply changes to prefab
	}

	scene->transforms.at(GUID).GUI();

	if (parts & Parts::rigidBody) { scene->rigidBodies.at(GUID).GUI(); }

	
	
	if (parts & Parts::pointLight) { scene->pointLights[GUID].GUI(); }
	if (parts & Parts::spotlight) { scene->spotlights[GUID].GUI(); }
	

	std::string addPopup = "SceneObject Add Part" + tag;
	std::string removePopup = "SceneObject Remove Part" + tag;

	if (ImGui::Button(("Add Part##" + tag).c_str())) {
		ImGui::OpenPopup(addPopup.c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button(("Remove Part##" + tag).c_str())) {
		ImGui::OpenPopup(removePopup.c_str());
	}

	if (ImGui::BeginPopup(addPopup.c_str())) {

		AddPartGUI(pointLight, setPointLight, PointLight, ("Point Light##Add part" + tag).c_str());
	
		AddPartGUI(rigidbody, setRigidBody, RigidBody(), ("Rigid Body##Add part" + tag).c_str());
		
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(removePopup.c_str())) {
		RemovePartGUI(animator, setAnimator, ("Animator##Remove part" + tag).c_str());
		RemovePartGUI(pointLight, setPointLight, ("Point Light##Remove part" + tag).c_str());
		RemovePartGUI(rigidBody, setRigidBody, ("Rigid Body##Remove part" + tag).c_str());
		
		RemovePartGUI(spotlight, setSpotlight, ("Spotlight ##Remove part" + tag).c_str());

		ImGui::EndPopup();
	}
}


void SceneObject::MenuGUI()
{
	std::string tag = Utilities::PointerToString(this);

	if (ImGui::MenuItem(("Delete##RightClick" + tag).c_str())) {
		scene->DeleteSceneObjectAndChildren(GUID);
	}
	if (ImGui::MenuItem((("Duplicate##RightClick") + tag).c_str())) {
		Duplicate();
	}
	if (ImGui::MenuItem(("Add empty child##" + tag).c_str())) {
		(new SceneObject(scene))->transform()->setParent(transform());
	}
	if (ImGui::MenuItem(("Save As Prefab##RightClick" + tag).c_str())) {
		SaveAsPrefab();
	}
	if (ImGui::MenuItem(("Replace with Prefab##RightClick" + tag).c_str())) {
		auto prefab = PrefabManager::loadedPrefabOriginals.find(PrefabManager::selectedPrefab);
		if (prefab != PrefabManager::loadedPrefabOriginals.end()) {
			LoadFromPrefab(prefab->second);
		}
		// Else // TODO: Warning
	}
	if (ImGui::MenuItem(("Refresh Prefab Instance##RightClick" + tag).c_str(), nullptr, nullptr, prefabStatus == PrefabStatus::instance)) {
		RefreshPrefab();
	}
	if (ImGui::MenuItem(("Unlink Prefab Instance##RightClick" + tag).c_str(), nullptr, nullptr, prefabStatus == PrefabStatus::instance)) {
		prefabStatus = PrefabStatus::missing;
	}
	if (ImGui::MenuItem(("Copy GUID##RightClick" + tag).c_str())) {
		ImGui::SetClipboardText(std::to_string(GUID).c_str());
	}
	auto prefabGUID = 0ull;
	if (prefabStatus == PrefabStatus::origin) {
		prefabGUID = GUID;
	}
	else if (prefabStatus == PrefabStatus::instance) {
		prefabGUID = prefabBase;
	}
	if (prefabGUID) {
		if (ImGui::MenuItem(("Select Prefab##RightClick" + tag).c_str())) {
			PrefabManager::selectedPrefab = prefabGUID;
		}
	}
	ImGui::EndPopup();

}

void SceneObject::MultiMenuGUI(std::set<SceneObject*> multiSelectedSceneObjects, bool* setNullSelect)
{
	std::string tag = "MultipleSelectRightClickSceneObjectMenu";

	if (ImGui::MenuItem(("Delete##RightClick" + tag).c_str())) {
		for (auto i : multiSelectedSceneObjects)
		{
			SceneManager::scene->DeleteSceneObjectAndChildren(i->GUID);
		}
		*setNullSelect = true;
	}
	if (ImGui::MenuItem(("Save Any Prefab Origins##RightClick" + tag).c_str())) {
		for (auto i : multiSelectedSceneObjects)
		{
			if (i->prefabStatus == SceneObject::PrefabStatus::origin) {
				i->SaveAsPrefab();
			}
		}
	}
	if (ImGui::MenuItem(("Refresh Prefab Instances##RightClick" + tag).c_str())) {
		for (auto i : multiSelectedSceneObjects) {
			i->RefreshPrefab();
		}
	}
	if (ImGui::MenuItem(("Replace with Prefab##RightClick" + tag).c_str())) {
		auto prefab = PrefabManager::loadedPrefabOriginals.find(PrefabManager::selectedPrefab);
		if (prefab != PrefabManager::loadedPrefabOriginals.end()) {
			for (auto i : multiSelectedSceneObjects) {
				i->LoadFromPrefab(prefab->second);
			}
		}
	}
	if (ImGui::MenuItem(("Unlink Any Prefabs##RightClick" + tag).c_str())) {
		for (auto i : multiSelectedSceneObjects) {
			if (i->prefabStatus == PrefabStatus::instance) {
				i->prefabStatus = PrefabStatus::missing;
			}
		}
	}
	ImGui::EndPopup();
}

bool SceneObject::PartsFilterSelector(const std::string& label, unsigned int& parts)
{
	std::string tag = Utilities::PointerToString(&parts);
	bool partsFilterOpen = ImGui::Button((label + "##Parts filter" + tag).c_str());
	std::string partsFilterPopupID = "partsFilterPopUp" + tag;
	if (parts) {
		ImGui::PopStyleColor();
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::Text("Parts Filter");
		ImGui::EndTooltip();
	}
	if (partsFilterOpen) {
		ImGui::OpenPopup(partsFilterPopupID.c_str());
	}
	if (!ImGui::BeginPopup(partsFilterPopupID.c_str())) {
		return false;
	}

	ImGui::CheckboxFlags("Animator##Parts Filter", &parts, Parts::animator);
	ImGui::CheckboxFlags("PointLight##Parts Filter", &parts, Parts::pointLight);
	ImGui::CheckboxFlags("RigidBody##Parts Filter", &parts, Parts::rigidBody);
	ImGui::CheckboxFlags("Spotlight##Parts Filter", &parts, Parts::spotlight);

	ImGui::EndPopup();
	return true;
}

void SceneObject::DebugDraw()
{
	Transform* t = &scene->transforms.at(GUID);
	if (parts & Parts::rigidBody)
	{
		scene->rigidBodies.at(GUID).DebugDraw(t);
	}
	//float s = 300.0f;
	//Transform* t = transform();
	//RenderSystem::debugLines.DrawLineSegment(t->getGlobalPosition(), t->getGlobalPosition() + t->right() * s, { 1, 0, 0 });
	//RenderSystem::debugLines.DrawLineSegment(t->getGlobalPosition(), t->getGlobalPosition() + t->up() * s, { 0, 1, 0 });
	//RenderSystem::debugLines.DrawLineSegment(t->getGlobalPosition(), t->getGlobalPosition() + t->forward() * s, { 0, 0, 1 });
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
		{ "parts", Serialisation::SaveAsUnsignedIntOLD(parts) },
		{ "parent", Serialisation::SaveAsUnsignedLongLong(parentGUID)},
		{ "children", childrenGUIDs },
		{ "prefabStatus", Serialisation::SaveAsUnsignedIntOLD((unsigned int)prefabStatus)},
		{ "prefabBase", Serialisation::SaveAsUnsignedLongLong(prefabBase) }
	};
}

toml::table SceneObject::SerialiseWithParts() const
{
	auto safetyCheck = parts;

	toml::table table;
	table.emplace("sceneObject", Serialise());

	SavePart("rigidBody", rigidBody, rigidBodies)
	SavePart("spotlight", spotlight, spotlights); 
	SavePart("pointLight", pointLight, pointLights);

	table.emplace("transform", transform()->Serialise(GUID));

	// TODO: Probably don't need a whole assert here, could just print a error and continue on
	assert(safetyCheck == 0);

	return table;
}

toml::table SceneObject::SerialiseWithPartsAndChildren() const
{
	toml::table table = SerialiseWithParts();

	toml::array savingChildren;
	auto children = transform()->getChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		savingChildren.push_back(children.at(i)->so->SerialiseWithPartsAndChildren());
	}

	table.emplace("children", savingChildren);

	return table;
}

SceneObject::SceneObject(Scene* _scene, toml::table* table) :
	scene(_scene)
{
	name = Serialisation::LoadAsString((*table)["name"]);
	GUID = Serialisation::LoadAsUnsignedLongLong((*table)["guid"]);
	parts = Serialisation::LoadAsUnsignedIntOLD((*table)["parts"]);
	scene->transforms[GUID].so = this;
	scene->sceneObjects[GUID] = this;
	prefabStatus = (PrefabStatus)Serialisation::LoadAsUnsignedIntOLD((*table)["prefabStatus"]);
	prefabBase = Serialisation::LoadAsUnsignedLongLong((*table)["prefabBase"]);
}

Transform* SceneObject::transform() const
{
	return &(scene->transforms.at(GUID));
}

// TODO: There is a case for the scene load to ensure that the below is also matched on scene load
// TODO: Try to remove any different special set / get
SetAndGetForPart(RigidBody, rigidBodies, Parts::rigidBody, RigidBody, rigidbody)
SetAndGetForPart(PointLight, pointLights, Parts::pointLight, PointLight, pointLight)
void SceneObject::setSpotlight(Spotlight* part) {
	if (part) {
		parts |= Parts::spotlight; scene->spotlights[GUID] = std::move(*part);
	}
	else {
		parts &= ~Parts::spotlight; scene->spotlights.erase(GUID);
	};
} Spotlight* SceneObject::spotlight() {
	if (parts & Parts::spotlight) {
		return &(scene->spotlights.at(GUID));
	} return nullptr;
}

void SceneObject::ClearParts(unsigned int toDelete)
{
	// TODO: If animator removed the model render needs to know about it
	if (toDelete & parts & Parts::rigidBody) { scene->rigidBodies.erase(GUID);   parts &= ~(Parts::rigidBody); }
	if (toDelete & parts & Parts::pointLight) { scene->pointLights.erase(GUID);	 parts &= ~(Parts::pointLight); }
	if (toDelete & parts & Parts::spotlight) { scene->spotlights.erase(GUID);	 parts &= ~(Parts::spotlight); }
}

void SceneObject::ClearParts()
{
	ClearParts(Parts::ALL);
	assert(parts == 0);
}

void SceneObject::SaveAsPrefab()
{
	prefabStatus = PrefabStatus::origin;

	std::ofstream file(Paths::prefabsSaveLocation + name + Paths::prefabExtension);

	toml::table table =	SerialiseWithPartsAndChildren();

	file << table << '\n';

	file.close();

	PrefabManager::loadedPrefabOriginals[GUID] = table;
}

void SceneObject::LoadWithParts(toml::table table)
{
	ClearParts();

	toml::table sceneObjectTable = *table["sceneObject"].as_table();
	prefabBase = Serialisation::LoadAsUnsignedLongLong(sceneObjectTable["guid"]);
	prefabStatus = (PrefabStatus)Serialisation::LoadAsUnsignedIntOLD(sceneObjectTable["prefabStatus"]);
	if (prefabStatus == PrefabStatus::origin) {
		prefabStatus = PrefabStatus::instance;
	}

	name = Serialisation::LoadAsString(sceneObjectTable["name"]);

	unsigned long long intendedParts = Serialisation::LoadAsUnsignedIntOLD(sceneObjectTable["parts"]);
	
	LoadPart("rigidBody", rigidBody, setRigidBody, RigidBody);
	LoadPart("spotlight", spotlight, setSpotlight, Spotlight);
	LoadPart("pointLight", pointLight, setPointLight, PointLight);

	auto loadingTransform = table["transform"];
	if (!loadingTransform) {
		// Transform info wasn't saved, this is okay for old prefabs
	}
	else {
		transform()->Load(*loadingTransform.as_table());
	}

	// TODO: Don't need a whole assert here
	assert(intendedParts == parts);
}

void SceneObject::LoadWithPartsSafe(toml::table table)
{
	// Remember information that should be kept
	std::string exitLevel = "";
	std::string pointLightTag = "";
	if (parts & Parts::pointLight) {
		pointLightTag = pointLight()->triggerTag;
	}
	std::string spotlightTag = "";
	if (parts & Parts::spotlight) {
		spotlightTag = spotlight()->triggerTag;
	}

	// Load parts
	LoadWithParts(table);

	// Apply saved information
	if (parts & Parts::pointLight && pointLightTag != "") {		
		pointLight()->triggerTag = pointLightTag;
	}
	if (parts & Parts::spotlight && spotlightTag != "") {
		spotlight()->triggerTag = spotlightTag;
	}

}

void SceneObject::LoadWithPartsSafeAndChildren(toml::table table)
{
	LoadWithPartsSafe(table);

	auto previousChildren = transform()->getChildren();
	auto temp = table["children"];
	if (!temp) {
		return;
	}

	auto loadingChildren = temp.as_array();
	if (loadingChildren->size() == 0) {
		return;
	}
	for (size_t i = 0; i < loadingChildren->size(); i++)
	{
		SceneObject* loadInto;
		if (i < previousChildren.size()) {
			// Load over the existing child object
			loadInto = previousChildren.at(i)->so;
		}
		else {
			// Create new sceneobject and load into that
			SceneObject* newChild = new SceneObject(scene);
			transform()->AddChild(newChild->transform());
			loadInto = newChild;
		}
		loadInto->LoadWithPartsSafeAndChildren(*loadingChildren->at(i).as_table());
	}
	// Clear extra children
	for (size_t i = loadingChildren->size(); i < previousChildren.size(); i++)
	{
		scene->DeleteSceneObjectAndChildren(previousChildren.at(i)->so->GUID);
	}
}

void SceneObject::LoadFromPrefab(toml::table table)
{
	// When loading directly from a prefab, the transform (of this object) should not be changed
	glm::mat4 originalTransform = transform()->getLocalMatrix();
	LoadWithPartsSafeAndChildren(table);
	prefabStatus = PrefabStatus::instance;
	transform()->setLocalMatrix(originalTransform);
}

void SceneObject::RefreshPrefab()
{
	auto search = PrefabManager::loadedPrefabOriginals.find(prefabBase);
	if (search != PrefabManager::loadedPrefabOriginals.end()) {
		LoadFromPrefab(PrefabManager::loadedPrefabOriginals.at(prefabBase));
	}
	else {
		prefabStatus = PrefabStatus::missing;
	}
}

void SceneObject::UnlinkFromPrefab()
{
	prefabStatus = PrefabStatus::missing;
}

void SceneObject::Duplicate() const
{
	SceneObject* newSceneObject = new SceneObject(scene);
	newSceneObject->LoadWithPartsSafeAndChildren(SerialiseWithPartsAndChildren());
	// Make the new object a sibling
	newSceneObject->transform()->setParent(transform()->getParent());
}
