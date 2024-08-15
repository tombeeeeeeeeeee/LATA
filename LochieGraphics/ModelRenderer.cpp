#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "SceneObject.h"
#include "ResourceManager.h"

#include "Utilities.h"

#include "imgui.h"	

ModelRenderer::ModelRenderer()
{
}

ModelRenderer::ModelRenderer(Model* _model, unsigned long long _materialGUID) :
	model(_model),
	modelGUID(_model->GUID)
{
	materialGUIDs.push_back(_materialGUID);
	materials.push_back(ResourceManager::GetMaterial(_materialGUID));

	materialGUIDs.resize(_model->materialIDs);
	materials.resize(_model->materialIDs);
}

ModelRenderer::ModelRenderer(Model* _model, Material* _material) :
	model(_model),
	modelGUID(_model->GUID)
{
	materialGUIDs.push_back(_material->GUID);
	materials.push_back(_material);

	materialGUIDs.resize(_model->materialIDs);
	materials.resize(_model->materialIDs);
}

void ModelRenderer::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::BeginDisabled();
	ImGui::DragInt(("Materials##" + tag).c_str(), &model->materialIDs);
	ImGui::EndDisabled();
	ImGui::Indent();
	// TODO: Make function for this
	for (size_t i = 0; i < materialGUIDs.size(); i++)
	{
		unsigned long long newMaterialGUID = materialGUIDs[i];
		if (ImGui::InputScalar((std::to_string(i) + "##" + Utilities::PointerToString(&materialGUIDs[i])).c_str(), ImGuiDataType_U64, &newMaterialGUID)) {
			Material* newMaterial = ResourceManager::GetMaterial(newMaterialGUID);
			if (newMaterial) {
				materialGUIDs[i] = newMaterialGUID;
				materials[i] = newMaterial;
			}
		}
	}
	ImGui::Unindent();
	// TODO: Ui for adding another material?

	unsigned long long newModelGUID = modelGUID;
	if (ImGui::InputScalar(("Model##" + Utilities::PointerToString(&modelGUID)).c_str(), ImGuiDataType_U64, &newModelGUID)) {
		Model* newModel = ResourceManager::GetModel(newModelGUID);
		if (newModel) {
			modelGUID = newModelGUID;
			model = newModel;
		}
	}
}

// TODO: Make sure to call this
void ModelRenderer::Refresh()
{
	model = ResourceManager::GetModel(modelGUID);
	materialGUIDs.resize(model->materialIDs);
	materials.resize(model->materialIDs);
	for (size_t i = 0; i < materialGUIDs.size(); i++)
	{
		materials[i] = ResourceManager::GetMaterial(materialGUIDs[i]);
	}
}

