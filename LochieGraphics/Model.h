#pragma once
#include "BoneInfo.h"
#include "ModelHierarchyInfo.h"

#include "assimp/scene.h"

#include <unordered_map>

class SceneObject;
class Mesh;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Model
{
	friend class ResourceManager;
private:
	Model();
	Model(std::string path);
	Model(toml::table table);

	static void HierarchyGUI(ModelHierarchyInfo* info);

public:
	glm::vec3 min;
	glm::vec3 max;

	std::string path = "";

	std::unordered_map<std::string, BoneInfo> boneInfoMap;

	ModelHierarchyInfo root;

	unsigned long long GUID;


	void LoadModel(std::string path);
	//void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?
	void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?

	const std::vector<Mesh*>& getMeshes() const;

	std::vector<Mesh*> meshes = {};
	std::vector<unsigned long long> meshGUIDs;

	int materialIDs = 1;

	void GUI();
	void OpenModal() {};

	void Refresh();

	operator std::string() const;
	std::string getDisplayName() const;

	static void ReadHierarchyData(ModelHierarchyInfo* dest, const aiNode* src);

	toml::table Serialise();
};

