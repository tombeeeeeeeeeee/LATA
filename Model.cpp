#include "Model.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include "assimp/postprocess.h"


Model::Model()
{
}

Model::Model(std::string path)
{
	LoadModel(path);
}

void Model::LoadModel(std::string path)
{
	const aiScene* scene = aiImportFile(path.c_str(), 0);

	if (scene == nullptr) {
		std::cout << "Error loading model at: " << path << "\n";
		throw;
	}
	else {
		std::cout << "Loaded model at: " << path << "\n";
	}

	meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes[i].InitialiseFromAiMesh(path, scene, mesh);
	}

	aiReleaseImport(scene);
}

void Model::Draw(Shader& shader)
{
	for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
	{
		mesh->Draw(shader);
	}
}
