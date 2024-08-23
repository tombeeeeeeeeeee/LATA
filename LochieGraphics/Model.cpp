#include "Model.h"

#include "Animation.h"

#include "ResourceManager.h"

#include "Utilities.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include "assimp/postprocess.h"

#include "imgui.h"
#include "imgui_stdlib.h"


Model::Model()
{
}

Model::Model(std::string _path, bool flipTexturesOnLoad) :
	path(_path)
{
	LoadModel(path, flipTexturesOnLoad);
}
// TODO: Models can no longer flip textures on load as they are always loaded seperatly now
void Model::LoadModel(std::string _path, bool flipTexturesOnLoad)
{
	path = _path;
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
	meshGUIDs.resize(scene->mNumMeshes);
	//meshes.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes[i] = ResourceManager::LoadMesh();
		meshGUIDs[i] = meshes[i]->GUID;
		meshes[i]->InitialiseFromAiMesh(path, scene, &boneInfoMap, mesh, flipTexturesOnLoad);
		min.x = std::min(meshes[i]->min.x, min.x);
		min.y = std::min(meshes[i]->min.y, min.y);
		max.x = std::max(meshes[i]->max.x, max.x);
		max.y = std::max(meshes[i]->max.y, max.y);
	}
	materialIDs = scene->mNumMaterials;

	Animation::ReadHierarchyData(&root, scene->mRootNode);
	// TODO: Do I need to do this, check exactly what should be done
	//aiReleaseImport(scene);
}

//void Model::AddMesh(Mesh* mesh)
//{
//	//meshes.emplace_back(std::move(*mesh));
//}
void Model::AddMesh(Mesh* mesh)
{
	meshes.push_back(mesh);
	meshGUIDs.push_back(mesh->GUID);
}


void Model::Draw()
{
	for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
	{
		(*mesh)->Draw();
	}
}

const std::vector<Mesh*>& Model::getMeshes() const
{
	return meshes;
}

void Model::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	std::string filename = path.substr(path.find_last_of("/\\") + 1);
	if (ImGui::CollapsingHeader((filename + "##" + tag).c_str())) {
		ImGui::Indent();

		ImGui::BeginDisabled();
		
		ImGui::InputScalar(("GUID##" + tag).c_str(), ImGuiDataType_U64, &GUID);
		ImGui::InputText(("Path##" + tag).c_str(), &path);
		int meshCount = meshes.size();
		ImGui::DragInt(("Mesh Count##" + tag).c_str(), &meshCount);

		ImGui::DragFloat2(("Min##" + tag).c_str(), &min.x);
		ImGui::DragFloat2(("Max##" + tag).c_str(), &max.x);

		ImGui::DragInt("Material IDs", &materialIDs);
		
		ImGui::EndDisabled();

		ImGui::Unindent();
	}
}

void Model::Refresh()
{
	for (size_t i = 0; i < meshGUIDs.size(); i++)
	{
		meshes[i] = ResourceManager::GetMesh(meshGUIDs[i]);
	}
}

std::string Model::getDisplayName()
{
	return root.name + " " + std::to_string(GUID);
}
