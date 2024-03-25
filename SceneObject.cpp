#include "SceneObject.h"

#include "Shader.h"
#include "Model.h"

#include "ModelRenderer.h"

SceneObject::SceneObject() :
	transform(Transform({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1.0f)),
	modelRenderer(nullptr)
{
}

SceneObject::SceneObject(Model* _model, Shader* _shader, glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	transform(Transform(_position, _rotation, _scale)),
	modelRenderer(new ModelRenderer(_model, _shader))
{
}

SceneObject::~SceneObject()
{
	delete modelRenderer;
}

void SceneObject::Draw() const
{
	if (!modelRenderer) { return; }
	modelRenderer->Draw(transform.getMatrix());
}