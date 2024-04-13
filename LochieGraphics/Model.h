#pragma once
#include "Mesh.h"
#include "Shader.h"

#include <map>

class Model
{
public:
	//struct BoneInfo
	//{
	//	// id is index in finalBoneMatrices
	//	int id;

	//	// offset matrix transforms vertex from model space to bone space
	//	glm::mat4 offset;

	//};
	////TODO: unordered map should be good?
	//std::map<std::string, BoneInfo> boneInfoMap;

	Model();
	Model(std::string path, bool flipTexturesOnLoad = true);

	void LoadModel(std::string path, bool flipTexturesOnLoad = true);
	void AddMesh(Mesh* mesh); // TODO: Maybe remove this function?
	void Draw();
private:
	//TODO: Model currently comepletly owns the meshes, this should not be the case
	std::vector<Mesh> meshes;
};

