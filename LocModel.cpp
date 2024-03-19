#include "LocModel.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include "assimp/postprocess.h"


LocModel::LocModel()
{
}

LocModel::LocModel(std::string path)
{
	LoadModel(path);
}

void LocModel::LoadModel(std::string path)
{
	const aiScene* scene = aiImportFile(path.c_str(), 0);

	//meshes.reserve(scene->mNumMeshes);
	meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		//meshes.push_back(Mesh(mesh));
		//meshes[i] = Mesh(scene, mesh);
		meshes[i].InitialiseFromAiMesh(path, scene, mesh);
	}

	aiReleaseImport(scene);
}

void LocModel::Draw(Shader& shader)
{
	for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
	{
		mesh->Draw(shader);
	}
}
