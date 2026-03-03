#include "SceneObject.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "Animator.h"
#include "Collider.h"
#include "ExitElevator.h"
#include "Triggerable.h"
#include "Sync.h"
#include "Ecco.h"
#include "Scene.h"
#include "Paths.h"
#include "PrefabManager.h"
#include "BlendedAnimator.h"
#include "Directional2dAnimator.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "LineRenderer.h"
#include "Serialisation.h"

#include <fstream>
#include <array>

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

#define PART_ENTRY(index, enumName, cls, collection, access, ignore, ...) \
	if (parts & Parts::enumName) { scene->collection.at(GUID)access PartGUI(); }

	ALL_PARTS
#undef PART_ENTRY

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


#define AddPartGUI(getter, setter, constructor, label) \
if (getter() == nullptr) {                             \
if (ImGui::MenuItem(label)) {                          \
		setter(new constructor);                       \
	}                                                  \
}

#define PART_ENTRY(index, lower, cls, collection, a, b, c, d, construct, ignore, ...) \
	AddPartGUI(lower, set##cls, construct(), (#cls "##Add part" + tag).c_str());

		ALL_PARTS;

#undef PART_ENTRY

		AddPartGUI(animator, setAnimator, BlendedAnimator(), ("Blended Animator##Add part" + tag).c_str());
		AddPartGUI(animator, setAnimator, Directional2dAnimator(), ("Directional 2D Animator##Add part" + tag).c_str());
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(removePopup.c_str())) {

#define PART_ENTRY(index, lower, cls, ignore, ...)                     \
		if (parts & Parts::lower)                                      \
		{                                                              \
			if (ImGui::MenuItem((#cls "##Remove part" + tag).c_str())) \
			{                                                          \
					set##cls(nullptr);                                 \
			}                                                          \
		}

		ALL_PARTS

#undef PART_ENTRY

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

	// TODO: These should be able to be sorted alphabetically instead
#define PART_ENTRY(index, lower, cls, ignore, ...) \
	{ ImGui::CheckboxFlags(#cls "##Parts Filter", &parts, Parts::lower); }
	ALL_PARTS
#undef PART_ENTRY

		ImGui::EndPopup();
	return true;
}

void SceneObject::DebugDraw()
{
	// TODO: Make this ignorant of the parts
	Transform* t = &scene->transforms.at(GUID);
	if (parts & Parts::rigidBody)
	{
		scene->rigidBodies.at(GUID).DebugDraw(t);
	}
	if (parts & Parts::collider)
	{
		scene->colliders.at(GUID)->DebugDraw(t);
	}
	if (parts & Parts::modelRenderer && UserPreferences::showSelectedBox) {
		Model* model = modelRenderer()->model;
		if (model) {
			LineRenderer& lines = RenderSystem::lines;
			lines.SetColour({ 1.0f, 1.0f, 1.0f });

			std::array<glm::vec3, 16> v{
				glm::vec3{ model->min.x, model->min.y, model->min.z },
				glm::vec3{ model->min.x, model->min.y, model->max.z },
				glm::vec3{ model->min.x, model->max.y, model->max.z },
				glm::vec3{ model->max.x, model->max.y, model->max.z },
				glm::vec3{ model->max.x, model->max.y, model->min.z },
				glm::vec3{ model->min.x, model->max.y, model->min.z },
				glm::vec3{ model->min.x, model->min.y, model->min.z },
				glm::vec3{ model->max.x, model->min.y, model->min.z },
				glm::vec3{ model->max.x, model->min.y, model->max.z },
				glm::vec3{ model->min.x, model->min.y, model->max.z },
				glm::vec3{ model->min.x, model->max.y, model->max.z },
				glm::vec3{ model->min.x, model->max.y, model->min.z },
				glm::vec3{ model->max.x, model->max.y, model->min.z },
				glm::vec3{ model->max.x, model->min.y, model->min.z },
				glm::vec3{ model->max.x, model->min.y, model->max.z },
				glm::vec3{ model->max.x, model->max.y, model->max.z },
			};
			for (size_t i = 0; i < v.size(); i++)
			{
				lines.AddPointToLine(glm::vec3(transform()->getGlobalMatrix() * glm::vec4(v.at(i), 1.0f)));
			}
			lines.FinishLineStrip();
		}
	}
	//float s = 300.0f;
	//Transform* t = transform();
	//RenderSystem::debugLines.DrawLineSegment(t->getGlobalPosition(), t->getGlobalPosition() + t->right() * s, { 1, 0, 0 });
	//RenderSystem::debugLines.DrawLineSegment(t->getGlobalPosition(), t->getGlobalPosition() + t->up() * s, { 0, 1, 0 });
	//RenderSystem::debugLines.DrawLineSegment(t->getGlobalPosition(), t->getGlobalPosition() + t->forward() * s, { 0, 0, 1 });
}

void SceneObject::TriggerCall(std::string tag, bool toggle)
{
	// TODO: Adjust this so things register for the call instead, or something alike
	if (parts & Parts::spawnManager)
		scene->spawnManagers[GUID].TriggerCall(tag, toggle);

	if (parts & Parts::door)
		scene->doors[GUID].TriggerCall(tag, toggle);

	if (parts & Parts::bollard)
		scene->bollards[GUID].TriggerCall(tag, toggle);

	if (parts & Parts::pointLight)
		scene->pointLights[GUID].TriggerCall(tag, toggle);

	if (parts & Parts::spotlight)
		scene->spotlights[GUID].TriggerCall(tag, toggle);
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


#define SavePart(saveName, partsName, container, access)                              \
	if (Parts::##partsName & parts) {                                                 \
		table.emplace(saveName, scene->##container.at(GUID) access Serialise(GUID));  \
		safetyCheck &= ~Parts::##partsName;                                           \
	}

#define PART_ENTRY(index, lower, cls, collection, access, ignore, ...) \
	SavePart(#lower, lower, collection, access)

	ALL_PARTS

#undef PART_ENTRY

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

void SceneObject::OnPartSet(Animator* part)
{
	if (parts & Parts::modelRenderer)
	{
		if (part)
		{
			modelRenderer()->animator = scene->animators.at(GUID);
		}
		else {
			modelRenderer()->animator = nullptr;
		}
	}
}

void SceneObject::OnPartSet(ModelRenderer* part)
{
	if (parts & Parts::animator && parts & Parts::modelRenderer)
	{
		modelRenderer()->animator = scene->animators.at(GUID);
	}
}

#define SetAndGetForPart(Type, container, enumValue, nameInSet, nameInGet, toStore, toGet) \
void SceneObject::set##nameInSet(Type* part)                                               \
{                                                                                          \
	if (part) {                                                                            \
		parts |= enumValue;                                                                \
 /*TODO: Ensure that this isn't leaking memory and is alright*/                            \
		scene->container[GUID] = toStore part;                                             \
	}                                                                                      \
	else {                                                                                 \
		parts &= ~enumValue;                                                               \
		scene->container.erase(GUID);                                                      \
	}                                                                                      \
	OnPartSet(part);                                                                       \
}                                                                                          \
Type * SceneObject::##nameInGet()                                                          \
{                                                                                          \
	if (parts & enumValue) {                                                               \
		return toGet(scene->container.at(GUID));                                           \
	}                                                                                      \
	return nullptr;                                                                        \
}


// TODO: There is a case for the scene load to ensure that the below is also matched on scene load

#define PART_ENTRY(index, lower, cls, container, access, rep, set, get, ignore, ...) \
SetAndGetForPart(cls, container, Parts::lower, cls, lower, set, get)

ALL_PARTS

#undef PART_ENTRY

void SceneObject::ClearParts(unsigned int toDelete)
{
#define PART_ENTRY(index, enumName, className, container, ignore, ...) \
	if (toDelete & parts & Parts::enumName) \
	{                                \
		scene->container.erase(GUID);        \
		parts &= ~(Parts::enumName);        \
	}
	ALL_PARTS
#undef PART_ENTRY
		//// TODO: If animator removed the model render needs to know about it
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

	toml::table table = SerialiseWithPartsAndChildren();

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


#define LoadPart(saveName, partsName, setter, type, v)      \
	if (intendedParts & Parts::##partsName) {               \
		/* TODO: likely leaking here, fix */                \
		setter(type::Load(*table[saveName].as_table()));    \
	}

#define PART_ENTRY(index, lower, cls, ignore, ...) \
	LoadPart(#lower, lower, set##cls, cls)

	ALL_PARTS;

#undef PART_ENTRY

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
	if (parts & Parts::exitElevator) {
		exitLevel = exitElevator()->levelToLoad;
	}
	bool hadSpawnManager = parts & Parts::spawnManager;
	toml::table spawnManagerData;
	if (hadSpawnManager) {
		spawnManagerData = spawnManager()->Serialise(GUID);
	}
	std::string pressurePlateTag = "";
	if (parts & Parts::plate) {
		pressurePlateTag = plate()->triggerTag;
	}
	std::string doorTag = "";
	if (parts & Parts::door) {
		doorTag = door()->triggerTag;
	}
	std::string bollardTag = "";
	if (parts & Parts::bollard) {
		bollardTag = bollard()->triggerTag;
	}
	std::string triggerableTag = "";
	if (parts & Parts::triggerable) {
		triggerableTag = triggerable()->triggerTag;
	}
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
	if (parts & Parts::exitElevator && exitLevel != "") {
		exitElevator()->levelToLoad = exitLevel;
	}
	if (parts & Parts::plate && pressurePlateTag != "") {
		plate()->triggerTag = pressurePlateTag;
	}
	if (parts & Parts::door && doorTag != "") {
		door()->triggerTag = doorTag;
	}
	if (parts & Parts::bollard && bollardTag != "") {
		bollard()->triggerTag = bollardTag;
	}
	if (parts & Parts::triggerable && triggerableTag != "") {
		triggerable()->triggerTag = triggerableTag;
	}
	if (parts & Parts::spawnManager && hadSpawnManager) {
		spawnManager()->Load(spawnManagerData);
	}
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
