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

		ImGui::ColorPicker3("Material Tint", &materialTint[0]);

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
		{ "materialTint", Serialisation::SaveAsVec3(materialTint)},
		{ "materials", savedMaterials },
	};
}

ModelRenderer::ModelRenderer(toml::table table)
{
	modelGUID = Serialisation::LoadAsUnsignedLongLong(table["modelGuid"]);
	materialTint = Serialisation::LoadAsVec3(table["materialTint"]);
	if (isnan(materialTint.x)) materialTint = {1.0f,1.0f,1.0f};
	toml::array* loadingMaterials = table["materials"].as_array();
	for (size_t i = 0; i < loadingMaterials->size(); i++)
	{
		materialGUIDs.push_back(Serialisation::LoadAsUnsignedLongLong(loadingMaterials->at(i)));
	}
	Refresh();
}

glm::vec3 ModelRenderer::GetMaterialOverlayColour()
{
	return Utilities::Lerp(materialTint, alternativeMaterialTint, tintDelta);
}

void ModelRenderer::setMaterialTint(glm::vec3 colour)
{
	materialTint = colour;
	if (materialTint.x > 1.0f || materialTint.y > 1.0f || materialTint.z > 1.0f)
		materialTint /= 255.0f;
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

