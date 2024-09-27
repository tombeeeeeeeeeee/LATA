#include "Model.h"

#include "Animation.h"
#include "Mesh.h"
#include "ResourceManager.h"

#include "Utilities.h"

#include "AssimpMatrixToGLM.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <iostream>

Model::Model()
{
}

Model::Model(std::string _path) :
	path(_path)
{
	LoadModel(path);
}
// TODO: Models can no longer flip textures on load as they are always loaded seperatly now
void Model::LoadModel(std::string _path)
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
		meshes[i]->InitialiseFromAiMesh(path, scene, &boneInfoMap, mesh);
		min.x = std::min(meshes[i]->min.x, min.x);
		min.y = std::min(meshes[i]->min.y, min.y);
		max.x = std::max(meshes[i]->max.x, max.x);
		max.y = std::max(meshes[i]->max.y, max.y);
	}
	materialIDs = scene->mNumMaterials;

	ReadHierarchyData(&root, scene->mRootNode);
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
		int meshCount = (int)meshes.size();
		ImGui::DragInt(("Mesh Count##" + tag).c_str(), &meshCount);

		ImGui::DragFloat2(("Min##" + tag).c_str(), &min.x);
		ImGui::DragFloat2(("Max##" + tag).c_str(), &max.x);

		ImGui::DragInt("Material IDs", &materialIDs);
		
		ImGui::EndDisabled();
		if (ImGui::CollapsingHeader(("Bone Info##" + tag).c_str())) {
			ImGui::Indent();
			for (auto& i : boneInfoMap)
			{
				if (ImGui::CollapsingHeader((i.first + "##" + tag).c_str())) {
					ExtraEditorGUI::Mat4Input(tag, &i.second.offset);
				}
			}
			if (boneInfoMap.empty()) {
				ImGui::Text("Empty!");
			}
			ImGui::Unindent();
		}

		HierarchyGUI(&root);
		ImGui::BeginDisabled();



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

Model::operator std::string() const
{
	return getDisplayName();
}

std::string Model::getDisplayName() const
{
	return Utilities::FilenameFromPath(path);
}

void Model::ReadHierarchyData(ModelHierarchyInfo* dest, const aiNode* src)
{
	if (!src) {
		std::cout << "Error, Failed to read animation hierarchy\n";
		return;
	}

	dest->name = src->mName.data;
	if (dest->name == "RootNode") {
		std::cout << "t";
	}

	aiVector3D pos;
	aiQuaternion rot;
	aiVector3D scale;

	src->mTransformation.Decompose(scale, rot, pos);
	dest->transform.getPosition() = AssimpVecToGLM(pos);
	dest->transform.setRotation(AssimpQuatToGLM(rot));
	dest->transform.setScale(AssimpVecToGLM(scale));

	//dest.transform.chi children.reserve(src->mNumChildren);

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		ModelHierarchyInfo* newData = new ModelHierarchyInfo();
		ReadHierarchyData(newData, src->mChildren[i]);

		dest->children.push_back(newData);

		newData->transform.setParent(&dest->transform);
	}
}

toml::table Model::Serialise()
{
	// The rest of model data can be reloaded with the path
	return toml::table{
		{ "path", path },
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
	};
}

Model::Model(toml::table table)
{
	GUID = Serialisation::LoadAsUnsignedLongLong(table["guid"]);
	LoadModel(Serialisation::LoadAsString(table["path"]));
}

void Model::HierarchyGUI(ModelHierarchyInfo* info)
{
	std::string tag = Utilities::PointerToString(info);
	if (ImGui::CollapsingHeader((info->name + "##" + tag).c_str())) {
		ImGui::Indent();
		info->transform.GUI();

		for (auto i : info->children)
		{
			HierarchyGUI(i);
		}
		ImGui::Unindent();
	}
}

