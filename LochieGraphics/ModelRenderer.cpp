#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"
#include "SceneObject.h"

#include "imgui.h"

ModelRenderer::ModelRenderer(Model* _model, Shader* _shader) :
	model(_model),
	shader(_shader)
{
}

void ModelRenderer::Draw()
{
	shader->Use();
	shader->setMat4("model", sceneObject->transform.getMatrix());
	model->Draw(shader);
}

void ModelRenderer::GUI()
{
	ImGui::Text(("Shader: " + std::to_string(shader->ID)).c_str());
}

