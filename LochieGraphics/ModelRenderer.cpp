#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "SceneObject.h"
#include "ResourceManager.h"

#include "imgui.h"

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

void ModelRenderer::Draw()
{
	material->Use();
	material->getShader()->setMat4("model", sceneObject->transform.getGlobalMatrix());
	model->Draw();
}

void ModelRenderer::GUI()
{
	// TODO:
	ImGui::Text(("Shader: " + std::to_string(material->getShader()->GLID)).c_str());
}

