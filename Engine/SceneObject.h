#pragma once

#include "PartsList.h"

#include <string>

#include <set>

#define PART_ENTRY(index, lower, cls, container, access, deref, a, b, construct, classOrStruct, ignore, ...) \
classOrStruct cls;

ALL_PARTS

#undef PART_ENTRY

class Scene;
class Shader;
class Transform;
class LineRenderer;
class Decal;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

// TODO: There might be issues later due to the parts being saved as an signed int, not unsigned
// Changing how they are saved can affect other saves and isn't worth it at the moment of writing

enum Parts : unsigned int
{
#define PART_ENTRY(index, enumName, ignore, ...) enumName = 1 << index,
	ALL_PARTS
#undef PART_ENTRY

#define PART_ENTRY(index, enumName, ignore, ...) | (1 << index)
	ALL = 0 ALL_PARTS,
#undef PART_ENTRY
};

class Scene;

class SceneObject
{
public:
	enum class PrefabStatus : unsigned int{
		none = 0,
		origin = 1,
		instance = 2,
		missing = 3,
	};

	std::string name = "Unnamed Scene Object";
	unsigned long long GUID = 0ull;
	unsigned int parts = 0u;
	Scene* scene = nullptr;

	PrefabStatus prefabStatus = PrefabStatus::none;
	unsigned long long prefabBase = 0ull;

	SceneObject(Scene* _scene, std::string name = "Unnamed Scene Object");
	SceneObject(Scene* _scene, toml::table* table);

	~SceneObject();

	//SceneObject(const SceneObject& other) = delete;
	//SceneObject& operator=(const SceneObject& other) = delete;

	void GUI();
	// This is the right click menu
	void MenuGUI();
	static void MultiMenuGUI(std::set<SceneObject*> multiSelectedSceneObjects, bool* setNullSelect);

	static bool PartsFilterSelector(const std::string& label, unsigned int& parts);

	void DebugDraw();

	void TriggerCall(std::string tag, bool toggle);

	toml::table Serialise() const;
	toml::table SerialiseWithParts() const;
	toml::table SerialiseWithPartsAndChildren() const;

	void SaveAsPrefab();
	void LoadWithParts(toml::table table);
	// Keeps certain information about the sceneobjects parts that shouldn't be replaced (like tags)
	void LoadWithPartsSafe(toml::table table);
	void LoadWithPartsSafeAndChildren(toml::table table);
	void LoadFromPrefab(toml::table table);
	void RefreshPrefab();

	void UnlinkFromPrefab();

	void Duplicate() const;

#pragma region Part Get and Set
	Transform* transform() const;

#define PART_ENTRY(index, lowerName, cls, container, access, pointer, ignore, ...) \
void set##cls(cls* lowerName);

	ALL_PARTS

#undef PART_ENTRY

#define PART_ENTRY(index, lowerName, cls, container, acess, pointer, ignore, ...) \
cls * lowerName();

	ALL_PARTS

#undef PART_ENTRY

#pragma endregion

	void ClearParts();
	void ClearParts(unsigned int toDelete);

private:
	template<typename T>
	void OnPartSet(T* partPntr)
	{}

	void OnPartSet(Animator* part);
	void OnPartSet(ModelRenderer* part);
};



