#include "Model.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include "assimp/postprocess.h"


Model::Model()
{
}

Model::Model(std::string path, bool flipTexturesOnLoad)
{
	LoadModel(path, flipTexturesOnLoad);
}

void Model::LoadModel(std::string path, bool flipTexturesOnLoad)
{
	const aiScene* scene = aiImportFile(path.c_str(), Mesh::aiLoadFlag);

	if (scene == nullptr) {
		std::cout << "Error loading model at: " << path << "\n";
		throw;
	}
	else {
		std::cout << "Loaded model at: " << path << "\n";
	}

	meshes.resize(scene->mNumMeshes);
	//meshes.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes[i].InitialiseFromAiMesh(path, scene, mesh, flipTexturesOnLoad);
		//Mesh newMesh;
		//newMesh.InitialiseFromAiMesh(path, scene, mesh);
		//meshes.push_back(std::move(newMesh));
	}

	aiReleaseImport(scene);
}

void Model::AddMesh(Mesh* mesh)
{
	meshes.push_back(std::move(*mesh));
}

void Model::Draw()
{
	material->Use();
	for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
	{
		mesh->Draw();
	}
}
