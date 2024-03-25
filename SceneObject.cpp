#include "SceneObject.h"

#include "Shader.h"
#include "Model.h"



SceneObject::SceneObject() :
	model(nullptr),
	shader(nullptr),
	transform(Transform({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1.0f))
{
}

SceneObject::SceneObject(Model* _model, Shader* _shader, glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	model(_model),
	shader(_shader),
	transform(Transform(_position, _rotation, _scale))
{
}

void SceneObject::Draw() const
{
	shader->Use();
	shader->setMat4("model", transform.getMatrix());
	model->Draw(shader);
}