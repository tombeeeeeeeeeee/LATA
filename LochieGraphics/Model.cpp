#include "Model.h"

#include "Animation.h"

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
// TODO: Models can no longer flip textures on load as they are always loaded seperatly now
void Model::LoadModel(std::string path, bool flipTexturesOnLoad)
{
	Assimp::Importer importer;

	//const aiScene* scene = aiImportFile(path.c_str(), Mesh::aiLoadFlag);



	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* scene = importer.ReadFile(path.c_str(), Mesh::aiLoadFlag);

	if (scene == nullptr) 
	{
		// TODO: assimp has error messages, get it and put it here
		std::cout << "Error loading model at: " << path << "\n";
		throw; // TODO: Do not throw here
	}
	else 
	{
		std::cout << "Loaded model at: " << path << "\n";
	}

	meshes.resize(scene->mNumMeshes);
	//meshes.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes[i].InitialiseFromAiMesh(path, scene, &boneInfoMap, mesh, flipTexturesOnLoad);
	}

	Animation::ReadHierarchyData(&root, scene->mRootNode);
	// TODO: Do I need to do this, check exactly what should be done
	//aiReleaseImport(scene);
}

//void Model::AddMesh(Mesh* mesh)
//{
//	//meshes.emplace_back(std::move(*mesh));
//}
void Model::AddMesh(Mesh mesh)
{
	meshes.push_back(mesh);
}


void Model::Draw()
{
	for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
	{
		mesh->Draw();
	}
}

const std::vector<Mesh>& Model::getMeshes() const
{
	return meshes;
}
