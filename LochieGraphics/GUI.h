#pragma once

#include "Maths.h"

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


	std::string hierarchyMenuSearch = "";



	std::vector<glm::mat4> selectedObjectLocalMatrixHistory = {};
	unsigned int selectedObjectHistoryIndex = 0;
	unsigned int multiSceneObjectEditorPartsFilter = 0;


	// ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize
	int defaultWindowFlags = 64;
	// ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	int baseNodeFlags = 128 | 64 | 2048;
	// ImGuizmo::OPERATION::TRANSLATE
	unsigned int transformGizmoOperation = 7;
	// ImGuizmo::MODE::WORLD
	unsigned int transformGizmoMode = 1;
	bool openMultiSelectRightClickMenu = false;
	bool focusSceneObjectMenu = false;
	bool gizmoChanged = false;
	bool zPressed = false;
	bool draggingSceneObject = false;
	bool draggingCheck = false;
	bool guizmoOperationChanged = false;
	bool guizmoModeChanged = false;
	bool disableGUIHeld = false;

public:
	Scene* scene = nullptr;

	SceneObject* lastSelected = nullptr;
	unsigned int partsFilter = 0u;
	int moveSelection = 0;
	
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
	bool showStyleMenu = false;


	void Update();

	SceneObject* getSelected() const;
	const std::set<SceneObject*>& getMultiSelected() const;
	void setSelected(SceneObject* so);
	void setSelected(std::vector<SceneObject*> sceneObjects);

	bool isObjectSelectedOrMultiSelected(SceneObject* so) const;

private:



	void ResourceMenu();

	void TestMenu();
	
	void CameraMenu();
	
	//bool showSceneObjectMenu = false;
	void SceneObjectMenu();
	
	void LightMenu();
	
	//bool showHierarchyMenu = false;
	void HierarchyMenu();
	void TransformTree(SceneObject* sceneObject);
	void TransformTree(SceneObject* so, ModelHierarchyInfo* model);
	void TransformDragDrop(SceneObject* sceneObject);
	void AddFromToSelection(SceneObject* from, SceneObject* to);
	void UpdateSelection();
	void MultiSceneObjectRightClickMenu();

	void MultiSceneObjectEditor();
};

