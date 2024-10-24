#include "ModelHierarchyInfo.h"

#include "EditorGUI.h"
#include "Utilities.h"

void ModelHierarchyInfo::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (!ImGui::CollapsingHeader((name + "##" + tag).c_str())) {
		return;
	}

	ImGui::Indent();
	transform.GUI();

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
		i->GUI();
	}
	ImGui::Unindent();
}

bool ModelHierarchyInfo::GlobalMatrixOfMesh(unsigned int meshIndex, Model* model, glm::mat4& matrix)
{
	if (std::find(meshes.begin(), meshes.end(), meshIndex) != meshes.end()) {
		matrix = transform.getGlobalMatrix();
		return true;
	}
	for (auto i : children)
	{
		if (i->GlobalMatrixOfMesh(meshIndex, model, matrix)) {
			return true;
		}
	}
	return false;
}
