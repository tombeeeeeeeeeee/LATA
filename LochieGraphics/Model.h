#pragma once
#include "Mesh.h"
#include "BoneInfo.h"
#include "Shader.h"
#include "ModelHierarchyInfo.h"

#include <unordered_map>

class SceneObject;

class Model
{
	friend class ResourceManager;
private:
	Model();
	Model(std::string path, bool flipTexturesOnLoad = true);


public:
	glm::vec3 min;
	glm::vec3 max;

	std::string path = "";

	std::unordered_map<std::string, BoneInfo> boneInfoMap;

	ModelHierarchyInfo root;

	unsigned long long GUID;


	void LoadModel(std::string path, bool flipTexturesOnLoad = true);
	//void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?
	void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?

	void Draw();
	const std::vector<Mesh*>& getMeshes() const;

	// TODO: Just store a vector of pairs, with both a mesh pointer and a GUID
	//TODO: Model currently comepletly owns the meshes, this should not be the case, actually with recent changed yes but there should be a model manager
	std::vector<Mesh*> meshes = {};
	std::vector<unsigned long long> meshGUIDs;

	int materialIDs = 1;

	void GUI();

	void Refresh();
};

