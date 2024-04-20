#pragma once
#include "Mesh.h"
#include "BoneInfo.h"
#include "Shader.h"

#include <unordered_map>

class SceneObject;

class Model
{
public:

	std::unordered_map<std::string, BoneInfo> boneInfoMap;

	Model();
	Model(std::string path, bool flipTexturesOnLoad = true);

	SceneObject* root;

	void LoadModel(std::string path, bool flipTexturesOnLoad = true);
	void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?
	void Draw();
	const std::vector<Mesh>& getMeshes() const;
private:
	//TODO: Model currently comepletly owns the meshes, this should not be the case, actually with recent changed yes but there should be a model manager
	std::vector<Mesh> meshes;
};

