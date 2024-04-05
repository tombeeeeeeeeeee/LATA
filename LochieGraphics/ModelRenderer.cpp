#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"
#include "SceneObject.h"

#include "imgui.h"

ModelRenderer::ModelRenderer(Model* _model) :
	model(_model)
{
}

void ModelRenderer::Draw()
{
	model->material->getShader()->Use();
	model->material->getShader()->setMat4("model", sceneObject->transform.getMatrix());
	model->Draw();
}

void ModelRenderer::GUI()
{
	// TODO:
	ImGui::Text(("Shader: " + std::to_string(model->material->getShader()->ID)).c_str());
}

