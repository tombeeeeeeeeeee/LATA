#include "MultiModelRenderer.h"

#include "Shader.h"
#include "Model.h"

void MultiModelRenderer::Draw(glm::mat4 matrix)
{
	shader->Use();
	for (auto i = transforms.begin(); i != transforms.end(); i++)
	{
		shader->setMat4("model", i->getMatrix() * matrix);
		model->Draw(shader);
	}
}

MultiModelRenderer::MultiModelRenderer(Model* _model, Shader* _shader) : ModelRenderer(_model, _shader)
{

}

MultiModelRenderer::MultiModelRenderer(Model* _model, Shader* _shader, std::vector<Transform> _tranforms) : ModelRenderer(_model, _shader),
	transforms(_tranforms)
{

}
