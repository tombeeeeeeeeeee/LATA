#include "MultiModelRenderer.h"

#include "Shader.h"
#include "Model.h"
#include "SceneObject.h"

#include "imgui.h"

void MultiModelRenderer::Draw()
{
	shader->Use();
	for (auto i = transforms.begin(); i != transforms.end(); i++)
	{
		//TODO: This moves in a unique way, kinda intersting maybe keep somehow
		shader->setMat4("model", i->getMatrix() * sceneObject->transform.getMatrix());
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

void MultiModelRenderer::GUI()
{
	ImGui::Text(("Shader: " + std::to_string(shader->ID)).c_str());
	for (unsigned int i = 0; i < transforms.size(); i++)
	{
		transforms[i].GUI();
	}
}
