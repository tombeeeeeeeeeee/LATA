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
	materialGUID(_materialGUID)
{
	material = ResourceManager::GetMaterial(materialGUID);
}

ModelRenderer::ModelRenderer(Model* _model, Material* _material) :
	model(_model),
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
	unsigned long long newMaterialGUID = materialGUID;
	if (ImGui::InputScalar(("Material##" + Utilities::PointerToString(&materialGUID)).c_str(), ImGuiDataType_U64, &newMaterialGUID)) {
		Material* newMaterial = ResourceManager::GetMaterial(newMaterialGUID);
		if (newMaterial) {
			materialGUID = newMaterialGUID;
			material = newMaterial;
		}
	}
}

