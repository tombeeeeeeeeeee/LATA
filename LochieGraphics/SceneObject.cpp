#include "SceneObject.h"

#include "ResourceManager.h"

#include "Shader.h"
#include "Model.h"

#include "imgui.h"
#include "imgui_stdlib.h"

SceneObject::SceneObject(Scene* _scene) :
	scene(_scene)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID] = Transform();
}

SceneObject::SceneObject(Scene* _scene, glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	scene(_scene)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID] = Transform(_position, _rotation, _scale);
}


SceneObject::~SceneObject()
{
	
}

void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	scene->transforms[GUID].GUI();

	if ((parts & Parts::modelRenderer))
		scene->renderers[GUID].GUI();
	
	//TODO Add animator parts;
	//if ((parts & Parts::animator)
	//	scene->animators[GUID].GUI();
}

void SceneObject::setTransform(Transform* transform)
{
	scene->transforms[GUID] = (*transform);
}

Transform* SceneObject::transform()
{
	return &(scene->transforms[GUID]);
}

void SceneObject::setRenderer(ModelRenderer* renderer)
{
	if (renderer)
	{
		parts |= Parts::modelRenderer;
		scene->renderers[GUID] = *renderer;
	}
	else
	{
		parts &= ~Parts::modelRenderer;
		scene->renderers.erase(GUID);
	}
}

ModelRenderer* SceneObject::renderer()
{
	if (parts & Parts::modelRenderer)
		return &(scene->renderers[GUID]);
	return nullptr;
}

//toml::table SceneObject::Serialise()
//{
//	return toml::table{
//		{ "name", name },
//		{ "hasRenderer", parts & Parts::modelRenderer}
//	};
//}
