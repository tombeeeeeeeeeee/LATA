#include "Model.h"

#include "Animation.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "Paths.h"

#include "Utilities.h"

#include "AssimpMatrixToGLM.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <iostream>
#include <fstream>

Model::Model()
{
}

Model::Model(std::string _path) :
	path(_path)
{
	LoadModel(path);
}

static void PrintMetaData(aiMetadata* metaData) {
	if (!metaData) { return; }
	for (size_t i = 0; i < metaData->mNumProperties; i++)
	{
		std::cout << "\nMeta data: " << metaData->mKeys[i].C_Str() << '\n';
		std::cout << "Type: ";

		aiString asString;
		if (metaData->mValues[i].mType == AI_AISTRING) {
			asString = ((aiString*)metaData->mValues[i].mData)->C_Str();
		}

		switch (metaData->mValues[i].mType)
		{
		case AI_BOOL:
			std::cout << "Bool\n" << *(bool*)metaData->mValues[i].mData;
			break;
		case AI_INT32:
			std::cout << "int 32\n" << *(int32_t*)metaData->mValues[i].mData;
			break;
		case AI_UINT64:
			std::cout << "u long long\n" << *(uint64_t*)metaData->mValues[i].mData;
			break;
		case AI_FLOAT:
			std::cout << "float\n" << *(float*)metaData->mValues[i].mData;
			break;
		case AI_DOUBLE:
			std::cout << "Double\n" << *(double*)metaData->mValues[i].mData;
			break;
		case AI_AISTRING:
			std::cout << "string\n" << (*(aiString*)metaData->mValues[i].mData).C_Str();
			break;
		case AI_AIVECTOR3D:
			std::cout << "vec3\n" << ((float*)metaData->mValues[i].mData)[0] << ", " << ((float*)metaData->mValues[i].mData)[1] << ", " << ((float*)metaData->mValues[i].mData)[2];
			break;
		case AI_AIMETADATA:
			std::cout << "meta meta data, idk how to read value";
			break;
		case AI_INT64:
			std::cout << "int 64\n" << *(int64_t*)metaData->mValues[i].mData;
			break;
		case AI_UINT32:
			std::cout << "u int\n" << *(uint32_t*)metaData->mValues[i].mData;
			break;
		case AI_META_MAX:
			std::cout << "meta max data, idk how to read value";
			break;
		default:
			break;
		}
		std::cout << "\n";
	}
}






// TODO: Models can no longer flip textures on load as they are always loaded seperatly now
void Model::LoadModel(std::string _path)
{
	path = _path;
	Assimp::Importer importer;

	//const aiScene* scene = aiImportFile(path.c_str(), Mesh::aiLoadFlag);


	// TODO: Look into this
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

	PrintMetaData(scene->mMetaData);
	

	//importer.ApplyPostProcessing(aiProcess_GlobalScale);
	//std::cout << "Scale factor: " << importer.GetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, AI_CONFIG_GLOBAL_SCALE_FACTOR_DEFAULT) << '\n';

	meshes.resize(scene->mNumMeshes);
	meshGUIDs.resize(scene->mNumMeshes);
	//meshes.reserve(scene->mNumMeshes);


	ReadHierarchyData(&root, scene->mRootNode);
	
	


	min.x = FLT_MAX;
	min.y = FLT_MAX;
	min.z = FLT_MAX;
	max.x = -FLT_MAX;
	max.y = -FLT_MAX;
	max.z = -FLT_MAX;

	auto temp = AssimpMatrixToGLM(scene->mRootNode->mTransformation);

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes[i] = ResourceManager::LoadMesh();
		meshGUIDs[i] = meshes[i]->GUID;
		meshes[i]->InitialiseFromAiMesh(path, scene, &boneInfoMap, mesh);
		min.x = std::min(meshes[i]->min.x, min.x);
		min.y = std::min(meshes[i]->min.y, min.y);
		min.z = std::min(meshes[i]->min.z, min.z);
		max.x = std::max(meshes[i]->max.x, max.x);
		max.y = std::max(meshes[i]->max.y, max.y);
		max.z = std::max(meshes[i]->max.z, max.z);
	}
	materialIDs = scene->mNumMaterials;

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

		ImGui::DragFloat3(("Min##" + tag).c_str(), &min.x);
		ImGui::DragFloat3(("Max##" + tag).c_str(), &max.x);

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

		root.GUI();
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
		std::cout << "Error, Failed to read model hierarchy\n";
		return;
	}


	//PrintMetaData(src->mMetaData);

	dest->name = src->mName.data;

	aiVector3D pos;
	aiQuaternion rot;
	aiVector3D scale;

	src->mTransformation.Decompose(scale, rot, pos);
	dest->transform.getPosition() = AssimpVecToGLM(pos);
	dest->transform.setRotation(AssimpQuatToGLM(rot));
	dest->transform.setScale(AssimpVecToGLM(scale));
	if (dest->name == "RootNode") {
		std::cout << "t";
		//dest->transform.setScale(dest->transform.getScale() * 100.0f);
	}
	
	for (size_t i = 0; i < src->mNumMeshes; i++)
	{
		dest->meshes.push_back(src->mMeshes[i]);
	}

	//dest.transform.chi children.reserve(src->mNumChildren);

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		dest->children.emplace_back(new ModelHierarchyInfo())->transform.setParent(&dest->transform);
		ReadHierarchyData(dest->children.back(), src->mChildren[i]);
	}
}

toml::table Model::Serialise() const
{
	// The rest of model data can be reloaded with the path
	return toml::table{
		{ "path", path },
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
	};
}

void Model::SaveAsAsset() const
{
	std::ofstream file(Paths::modelSaveLocation + Utilities::FilenameFromPath(path, false) + Paths::modelExtension);
	file << Serialise();
	file.close();
}

Model::Model(toml::table table)
{
	GUID = Serialisation::LoadAsUnsignedLongLong(table["guid"]);
	LoadModel(Serialisation::LoadAsString(table["path"]));
}
