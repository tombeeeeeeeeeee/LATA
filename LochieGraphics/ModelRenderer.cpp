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
	material = nullptr;
}

ModelRenderer::ModelRenderer(Model* _model, unsigned long long _materialGUID) :
	model(_model),
	modelGUID(_model->GUID),
	materialGUID(_materialGUID)
{
	material = ResourceManager::GetMaterial(materialGUID);
}

ModelRenderer::ModelRenderer(Model* _model, Material* _material) :
	model(_model),
	modelGUID(_model->GUID),
	material(_material)
{
	materialGUID = material->GUID;
}

void ModelRenderer::Draw(Shader* override)
{
	//if (!override/* || material->getShader() == override*/) {
	//	material->Use();
	//	material->getShader()->setMat4("model", sceneObject->transform().getGlobalMatrix());
	//}
	//else {
	//	override->setMat4("model", sceneObject->transform().getGlobalMatrix());
	//}
	//model->Draw();
}

void ModelRenderer::GUI()
{
	// TODO:
	ImGui::Text(("Model: " + Utilities::PointerToString(model)).c_str());

	std::string tag = Utilities::PointerToString(this);
	// TODO: Make function for this
	unsigned long long newMaterialGUID = materialGUID;
	if (ImGui::InputScalar(("Material##" + Utilities::PointerToString(&materialGUID)).c_str(), ImGuiDataType_U64, &newMaterialGUID)) {
		Material* newMaterial = ResourceManager::GetMaterial(newMaterialGUID);
		if (newMaterial) {
			materialGUID = newMaterialGUID;
			material = newMaterial;
		}
	}

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
	material = ResourceManager::GetMaterial(materialGUID);
}

