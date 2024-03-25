#include "ModelRenderer.h"

#include "Model.h"
#include "Shader.h"

ModelRenderer::ModelRenderer(Model* _model, Shader* _shader) :
	model(_model),
	shader(_shader)
{
}

void ModelRenderer::Draw(glm::mat4 matrix)
{
	shader->Use();
	shader->setMat4("model", matrix);
	model->Draw(shader);
}
