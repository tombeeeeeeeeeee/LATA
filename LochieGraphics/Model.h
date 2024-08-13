#pragma once
#include "Mesh.h"
#include "BoneInfo.h"
#include "Shader.h"
#include "ModelHierarchyInfo.h"

#include <unordered_map>

class SceneObject;

class Model
{
public:

	std::unordered_map<std::string, BoneInfo> boneInfoMap;

	ModelHierarchyInfo root;

	Model();
	Model(std::string path, bool flipTexturesOnLoad = true);

	void LoadModel(std::string path, bool flipTexturesOnLoad = true);
	void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?
	void Draw();
	const std::vector<Mesh>& getMeshes() const;

	//TODO: Model currently comepletly owns the meshes, this should not be the case, actually with recent changed yes but there should be a model manager
	std::vector<Mesh> meshes;
};

