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
}

ModelRenderer::ModelRenderer(Model* _model, unsigned long long _materialGUID) :
	model(_model),
	modelGUID(_model->GUID)
{
	materialGUIDs.push_back(_materialGUID);
	materials.push_back(ResourceManager::GetMaterial(_materialGUID));

	materialGUIDs.resize(_model->materialIDs);
	materials.resize(_model->materialIDs);
}

ModelRenderer::ModelRenderer(Model* _model, Material* _material) :
	model(_model),
	modelGUID(_model->GUID)
{
	materialGUIDs.push_back(_material->GUID);
	materials.push_back(_material);

	materialGUIDs.resize(_model->materialIDs);
	materials.resize(_model->materialIDs);
}

void ModelRenderer::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::BeginDisabled();
	ImGui::DragInt(("Materials##" + tag).c_str(), &model->materialIDs);
	ImGui::EndDisabled();
	ImGui::Indent();
	for (size_t i = 0; i < materialGUIDs.size(); i++)
	{
		// TODO: A better way to reference the general default shaders
		if (ResourceManager::MaterialSelector(std::to_string(i), &materials[i], sceneObject->scene->shaders[super], true)) {
			materialGUIDs[i] = materials[i]->GUID;
		}
	}
	ImGui::Unindent();

	if (ResourceManager::ModelSelector("Model", &model)) {
		modelGUID = model->GUID;
		Refresh();
	}
}

// TODO: Make sure to call this
void ModelRenderer::Refresh()
{
	model = ResourceManager::GetModel(modelGUID);
	materialGUIDs.resize(model->materialIDs);
	materials.resize(model->materialIDs);
	for (size_t i = 0; i < materialGUIDs.size(); i++)
	{
		materials[i] = ResourceManager::GetMaterial(materialGUIDs[i]);
	}
}

