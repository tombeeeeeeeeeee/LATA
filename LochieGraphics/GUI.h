#pragma once

#include <vector>
#include <set>
#include <string>

class Scene;
class SceneObject;
class ModelHierarchyInfo;

// TODO: Not all the menus need to have the word menu in their name
class GUI
{
private:
	SceneObject* sceneObjectSelected = nullptr;
	ModelHierarchyInfo* modelHierarchySelected = nullptr;

	std::vector<std::pair<SceneObject*, SceneObject*>> addRangeToSelection;

	std::vector<SceneObject*> hierarchySceneObjects = {};
	std::set<SceneObject*> multiSelectedSceneObjects = {};

	bool openMultiSelectRightClickMenu = false;

	std::string hierarchyMenuSearch = "";

public:
	Scene* scene = nullptr;

	SceneObject* lastSelected = nullptr;

	

	void Update();

	bool showResourceMenu = false;
	bool showTestMenu = false;
	bool showCameraMenu = false;
	bool showHierarchy = false;
	bool showSceneObject = false;
	bool showPhysicsMenu = false;
	bool showEnemyMenu = false;
	bool showLightMenu = false;
	bool showImguiExampleMenu = false;
	bool showRenderSystemMenu = false;
	bool showHealthSystemMenu = false;
	bool showUserPrefsMenu = false;
	bool showPrefabMenu = false;
	bool showParticleMenu = false;
	bool showAudioMenu = false;

	SceneObject* getSelected();
	void setSelected(SceneObject* so);

private:

	// ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize
	int defaultWindowFlags = 64;

	void ResourceMenu();

	void TestMenu();
	
	void CameraMenu();
	
	//bool showSceneObjectMenu = false;
	void SceneObjectMenu();
	
	int lightSelectedIndex = 0;
	void LightMenu();
	
	//bool showHierarchyMenu = false;
	void HierarchyMenu();
	void TransformTree(SceneObject* sceneObject);
	void TransformTree(SceneObject* so, ModelHierarchyInfo* model);
	void TransformDragDrop(SceneObject* sceneObject);
	// ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	int baseNodeFlags = 128 | 64 | 2048;

	bool operationChanged = false;
	// ImGuizmo::OPERATION::TRANSLATE
	unsigned int transformGizmoOperation = 7;

	void PhysicsMenu();
	void EnemyMenu();
	void HealthMenu();
	bool disableGUIHeld = false;

	void AddFromToSelection(SceneObject* from, SceneObject* to);
	void UpdateSelection();
	void MultiSceneObjectRightClickMenu();
};

