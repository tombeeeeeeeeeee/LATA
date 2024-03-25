#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"
#include "SceneObject.h"

#include "imguiStuff.h"

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

void ModelRenderer::GUI(Part* part)
{
	ModelRenderer* modelRenderer = (ModelRenderer*)part;
	ImGui::Text(("Shader: " + std::to_string(modelRenderer->shader->ID)).c_str());
}

