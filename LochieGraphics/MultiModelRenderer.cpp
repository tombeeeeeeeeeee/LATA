#include "MultiModelRenderer.h"

#include "Shader.h"
#include "Model.h"
#include "SceneObject.h"

#include "imgui.h"

void MultiModelRenderer::Draw()
{
	material->Use();
	for (auto i = transforms.begin(); i != transforms.end(); i++)
	{
		//TODO: This moves in a unique way, kinda intersting maybe keep somehow
		material->getShader()->setMat4("model", i->getMatrix() * sceneObject->transform.getMatrix());
		model->Draw();
	}
}

MultiModelRenderer::MultiModelRenderer(Model* _model, Material* _material) : ModelRenderer(_model, _material)
{

}

MultiModelRenderer::MultiModelRenderer(Model* _model, Material* _material, std::vector<Transform> _tranforms) : ModelRenderer(_model, _material),
	transforms(_tranforms)
{

}

void MultiModelRenderer::GUI()
{
	// TODO:
	ImGui::Text(("Shader: " + std::to_string(material->getShader()->ID)).c_str());
	for (unsigned int i = 0; i < transforms.size(); i++)
	{
		transforms[i].GUI();
	}
}
