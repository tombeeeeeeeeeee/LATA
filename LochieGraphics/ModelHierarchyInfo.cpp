#include "ModelHierarchyInfo.h"

#include "BoneInfo.h"

#include "Utilities.h"

#include "ExtraEditorGUI.h"

void ModelHierarchyInfo::GUI(bool header, std::unordered_map<std::string, BoneInfo>* boneInfoMap)
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
			if (ImGui::CollapsingHeader(("Bone Transform##" + tag).c_str())) {
				ImGui::Indent();
				ExtraEditorGUI::Mat4Input(tag, &search->second.offset);
				ImGui::Unindent();
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

bool ModelHierarchyInfo::ModelMatrixOfMesh(unsigned int meshIndex, glm::mat4& matrix)
{
	if (std::find(meshes.begin(), meshes.end(), meshIndex) != meshes.end()) {
		matrix = transform.getGlobalMatrix();
		return true;
	}
	for (auto i : children)
	{
		if (i->ModelMatrixOfMesh(meshIndex, matrix)) {
			return true;
		}
	}
	return false;
}
