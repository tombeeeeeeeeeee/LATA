#include "ModelHierarchyInfo.h"

#include "Animator.h"
#include "BoneInfo.h"

#include "Utilities.h"

#include "ExtraEditorGUI.h"

void ModelHierarchyInfo::GUI(bool header, std::unordered_map<std::string, BoneInfo>* boneInfoMap, Animator* animator)
{
	std::string tag = Utilities::PointerToString(this);

	if (header) {
		if (!ImGui::CollapsingHeader((name + "##" + tag).c_str())) {
			return;
		}
	}

	ImGui::Indent();
	transform.GUI();

	if (boneInfoMap) {
		auto search = boneInfoMap->find(name);
		if (search != boneInfoMap->end()) {
			if (ImGui::CollapsingHeader(("Bone Info Transform##" + tag).c_str())) {
				ImGui::Indent();
				ExtraEditorGUI::Mat4Input(tag + "bone info transform", &search->second.offset);
				ImGui::Unindent();
			}
			if (animator) {
				auto& boneMatrices = animator->getNonConstFinalBoneMatrices();
				
				if (ImGui::CollapsingHeader(("Animated Bone Transform##" + tag).c_str())) {
					ImGui::Indent();
					ExtraEditorGUI::Mat4Input(tag + "animated bone transform", &boneMatrices.at(search->second.ID));
				}

			}
		}
		else {
			ImGui::Text("No bone on this node");
		}
	}


	if (meshes.empty()) {
		ImGui::Text("No meshes directly on this node");
	}
	else if (ImGui::CollapsingHeader(("Meshes IDs##" + tag).c_str())) {
		
		for (size_t i = 0; i < meshes.size(); i++)
		{
			ImGui::Text(std::to_string(meshes.at(i)).c_str());
		}
	}

	for (auto i : children)
	{
		//i->GUI(true, boneInfoMap);
	}
	ImGui::Unindent();
}

bool ModelHierarchyInfo::ModelHierarchyInfoOfMesh(unsigned int meshIndex, ModelHierarchyInfo** info)
{
	if (std::find(meshes.begin(), meshes.end(), meshIndex) != meshes.end()) {
		*info = this;
		return true;
	}
	for (auto i : children)
	{
		if (i->ModelHierarchyInfoOfMesh(meshIndex, info)) {
			return true;
		}
	}
	return false;
}
