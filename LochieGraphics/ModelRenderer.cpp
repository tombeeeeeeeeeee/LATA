#include "ModelRenderer.h"

#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneObject.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

#include <iostream>

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

void ModelRenderer::Draw(glm::mat4 modelMatrix, Shader* givenShader)
{
	if (!model) { return; }

	Material* previousMaterial = nullptr;

	for (int i = 0; i < model->meshes.size(); i++)
	{
		Mesh* mesh = model->meshes.at(i);
		int materialID = mesh->materialID;
		Material* currentMaterial = nullptr;
		if (materialID >= materials.size()) continue;
		if (materialID >= model->materialIDs || materials.at(materialID) == nullptr) {
			materialID = 0;
			std::cout << "Invalid model material ID\n";
		}
		currentMaterial = materials.at(materialID);

		// Only bind material if using a different material
		if (currentMaterial != previousMaterial) {
			// Skip if no material is set
			if (currentMaterial == nullptr) { continue; }
			currentMaterial->Use(givenShader ? givenShader : nullptr);
			previousMaterial = currentMaterial;
		}

		Shader* shader;
		if (!givenShader) {
			Material* material = materials.at(materialID);
			// TODO: Maybe an error / warning

			if (!material) { continue; }
			shader = material->getShader();
		}
		else {
			shader = givenShader;
		}

		shader->Use();
		shader->setMat4("view", SceneManager::scene->renderSystem.viewMatrix);
		glm::mat4 subModel = glm::identity<glm::mat4>();

		if (animator) {
			ModelHierarchyInfo* info = nullptr;
			model->root.ModelHierarchyInfoOfMesh(i, &info);
			auto search = model->boneInfoMap.find(info->name);
			if (search == model->boneInfoMap.end()) {
				subModel = info->transform.getGlobalMatrix();
			}
			else {
				BoneInfo* boneInfo = &search->second;
				subModel = animator->getFinalBoneMatrices().at(boneInfo->ID);
			}
		}
		else {
			ModelHierarchyInfo* info = nullptr;
			model->root.ModelHierarchyInfoOfMesh(i, &info);
			subModel = info->transform.getGlobalMatrix();;
		}
		
		
		shader->setMat4("model", modelMatrix * subModel);
		SceneManager::scene->renderSystem.ActivateFlaggedVariables(shader, materials.at(materialID));

		glm::vec3 overallColour = materials.at(materialID)->colour * GetMaterialOverlayColour();
		shader->setVec3("materialColour", overallColour);

		if (animator) {
			std::vector<glm::mat4> boneMatrices = animator->getFinalBoneMatrices();
			for (int i = 0; i < boneMatrices.size(); i++)
			{
				shader->setMat4("boneMatrices[" + std::to_string(i) + "]", boneMatrices.at(i));
			}
		}
		mesh->Draw();
	}
}

void ModelRenderer::GUI()
{
	//ImGui::Text("");
	std::string tag = Utilities::PointerToString(this);
	if (!ImGui::CollapsingHeader(("Model Renderer##" + tag).c_str()))
	{
		return;
	}
	ImGui::Indent();
	ImGui::BeginDisabled();
	int mats = (int)materialGUIDs.size();
	bool animated = animator;
	ImGui::Checkbox(("Animated##" + tag).c_str(), &animated);
	ImGui::DragInt(("Materials##" + tag).c_str(), &mats);
	ImGui::EndDisabled();

	ImGui::ColorEdit3(("Material Tint##" + tag).c_str(), &materialTint[0]);

	ImGui::Indent();
	for (size_t i = 0; i < materialGUIDs.size(); i++)
	{
		if (ResourceManager::MaterialSelector("Material " + std::to_string(i), &materials[i], ResourceManager::defaultShader, true)) {
			if (materials[i] != nullptr) {
				materialGUIDs[i] = materials[i]->GUID;
			}
			else {
				materialGUIDs[i] = 0;
			}
		}
	}
	ImGui::Unindent();

	if (ResourceManager::ModelSelector("Model", &model)) {
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

