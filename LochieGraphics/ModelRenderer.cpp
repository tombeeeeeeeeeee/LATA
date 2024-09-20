#include "ModelRenderer.h"

#include "ResourceManager.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

ModelRenderer::ModelRenderer()
{
	Refresh();
}

ModelRenderer::ModelRenderer(Model* _model, unsigned long long _materialGUID) :
	model(_model),
	modelGUID(_model->GUID)
{
	materialGUIDs.push_back(_materialGUID);
	materials.push_back(ResourceManager::GetMaterial(_materialGUID));

	Refresh();
}

ModelRenderer::ModelRenderer(Model* _model, Material* _material) :
	model(_model),
	modelGUID(_model->GUID)
{
	materialGUIDs.push_back(_material->GUID);
	materials.push_back(_material);

	Refresh();
}

void ModelRenderer::GUI()
{
	//ImGui::Text("");
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader("Model Renderer"))
	{
		ImGui::Indent();
		ImGui::BeginDisabled();
		int mats = (int)materialGUIDs.size();
		ImGui::DragInt(("Materials##" + tag).c_str(), &mats);
		ImGui::EndDisabled();
		ImGui::Indent();
		for (size_t i = 0; i < materialGUIDs.size(); i++)
		{
			if (ResourceManager::MaterialSelector(std::to_string(i), &materials[i], ResourceManager::defaultShader, true)) {
				if (materials[i] != nullptr) {
					materialGUIDs[i] = materials[i]->GUID;
				}
				else {
					materialGUIDs[i] = 0;
				}
			}
		}
		ImGui::Unindent();

		if (ResourceManager::ModelAssetSelector("Model", &model)) {
			if (model) {
				modelGUID = model->GUID;
			}
			else {
				modelGUID = 0;
			}
			Refresh();
		}
		ImGui::Unindent();
	}
}

toml::table ModelRenderer::Serialise(unsigned long long GUID) const
{
	toml::array savedMaterials;
	for (auto& i : materialGUIDs)
	{
		savedMaterials.push_back(Serialisation::SaveAsUnsignedLongLong(i));
	}

	return toml::v3::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "modelGuid", Serialisation::SaveAsUnsignedLongLong(modelGUID)},
		{ "materials", savedMaterials },
	};
}

ModelRenderer::ModelRenderer(toml::table table)
{
	modelGUID = Serialisation::LoadAsUnsignedLongLong(table["modelGuid"]);
	toml::array* loadingMaterials = table["materials"].as_array();
	for (size_t i = 0; i < loadingMaterials->size(); i++)
	{
		materialGUIDs.push_back(Serialisation::LoadAsUnsignedLongLong(loadingMaterials->at(i)));
	}
	Refresh();
}

// TODO: Make sure to call this
void ModelRenderer::Refresh()
{
	model = ResourceManager::GetModel(modelGUID);
	if (model == nullptr) {
		materialGUIDs.resize(1);
		materials.resize(1);
	}
	else {
		materialGUIDs.resize(model->materialIDs);
		materials.resize(model->materialIDs);
	}
	for (size_t i = 0; i < materialGUIDs.size(); i++)
	{
		materials[i] = ResourceManager::GetMaterial(materialGUIDs[i]);
	}
}

