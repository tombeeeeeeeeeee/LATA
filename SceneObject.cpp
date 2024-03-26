#include "SceneObject.h"

#include "Shader.h"
#include "Model.h"

#include "ModelRenderer.h"

SceneObject::SceneObject() :
	transform(Transform({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1.0f))
{
}

SceneObject::SceneObject(glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	transform(Transform(_position, _rotation, _scale))
{
}

SceneObject::~SceneObject()
{
	// TODO: Cleanup parts, make sure working
	while (!parts.empty()) {
		delete parts.back();
		parts.pop_back();
	}
}

void SceneObject::Update(float delta)
{
	for (auto part = parts.begin(); part != parts.end(); part++)
	{
		(*part)->Update();
	}
}

void SceneObject::Draw() const
{
	for (auto part = parts.begin(); part != parts.end(); part++)
	{
		(*part)->Draw();
	}
}

void SceneObject::AddPart(Part* part)
{
	parts.push_back(part);
	part->sceneObject = this;
}
