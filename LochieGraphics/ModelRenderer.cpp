#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "SceneObject.h"

#include "imgui.h"

ModelRenderer::ModelRenderer(Model* _model, Material* _material) :
	model(_model),
	material(_material)
{
}

void ModelRenderer::Draw()
{
	material->Use();
	material->getShader()->setMat4("model", sceneObject->transform.getMatrix());
	model->Draw();
}

void ModelRenderer::GUI()
{
	// TODO:
	ImGui::Text(("Shader: " + std::to_string(material->getShader()->ID)).c_str());
}

