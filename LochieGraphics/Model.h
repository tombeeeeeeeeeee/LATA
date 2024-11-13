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
public:
	friend class ResourceManager;
	Model();
	Model(std::string path);
	static void PrintMetaData(aiMetadata* metaData);
	Model(toml::table table);

	glm::vec3 min = { 0.0f, 0.0f, 0.0f };
	glm::vec3 max = { 0.0f, 0.0f, 0.0f };

	glm::vec3* GetOOB(glm::mat4 modelMatrix);

	std::string path = "";

	std::unordered_map<std::string, BoneInfo> boneInfoMap;

	ModelHierarchyInfo root;

	unsigned long long GUID = 0;


	void LoadModel(std::string path);
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

	toml::table Serialise() const;
	void SaveAsAsset() const;

	
};

