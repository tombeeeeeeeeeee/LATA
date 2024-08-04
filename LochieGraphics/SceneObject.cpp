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
	scene->transforms[GUID] = Transform(this);
}

SceneObject::SceneObject(Scene* _scene, glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	scene(_scene)
{
	GUID = ResourceManager::GetNewGuid();
	scene->transforms[GUID] = Transform(this,_position, _rotation, _scale);
}


SceneObject::~SceneObject()
{
	
}

void SceneObject::Update(float delta)
{
}

void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	scene->transforms[GUID].GUI();

	if ((parts & Parts::modelRenderer))
		scene->renderers[GUID].GUI();
	
	if ((parts & Parts::rigidBody))
		scene->rigidBodies[GUID].GUI();

	//TODO Add animator parts;
	//if ((parts & Parts::animator))
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

void SceneObject::setAnimator(Animator* animator)
{
	if (animator)
	{
		parts |= Parts::animator;
		scene->animators[GUID] = *animator;
	}
	else
	{
		parts &= ~Parts::animator;
		scene->animators.erase(GUID);
	}
}

Animator* SceneObject::animator()
{
	if (parts & Parts::animator)
		return &(scene->animators[GUID]);
	return nullptr;
}

void SceneObject::setRigidBody(RigidBody* rb)
{
	if (rb)
	{
		parts |= Parts::rigidBody;
		scene->rigidBodies[GUID] = *rb;
	}
	else
	{
		parts &= ~Parts::rigidBody;
		scene->rigidBodies.erase(GUID);
	}
}

RigidBody* SceneObject::rigidbody()
{
	if (parts & Parts::rigidBody)
		return &(scene->rigidBodies[GUID]);
	return nullptr;
}

void SceneObject::setCollider(Collider* collider)
{
	if (collider)
	{
		parts |= Parts::collider;
		scene->colliders[GUID] = *collider;
	}
	else
	{
		parts &= ~Parts::collider;
		scene->colliders.erase(GUID);
	}
}

Collider* SceneObject::collider()
{
	if (parts & Parts::collider)
		return &(scene->colliders[GUID]);
	return nullptr;
}

//toml::table SceneObject::Serialise()
//{
//	return toml::table{
//		{ "name", name },
//		{ "hasRenderer", parts & Parts::modelRenderer}
//	};
//}
