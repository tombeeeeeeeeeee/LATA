#include "ModelHierarchyInfo.h"


#include "Utilities.h"

#include "ExtraEditorGUI.h"

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
