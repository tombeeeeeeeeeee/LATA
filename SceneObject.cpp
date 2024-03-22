#include "SceneObject.h"

#include "Shader.h"
#include "Model.h"

#include "gtc/matrix_transform.hpp"

SceneObject::SceneObject() :
	model(nullptr),
	shader(nullptr),
	position({ 0.f, 0.f, 0.f }),
	rotation({ 0.f, 0.f, 0.f}),
	scale(1.0f)
{
}

SceneObject::SceneObject(Model* _model, Shader* _shader, glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	model(_model),
	shader(_shader),
	position(_position),
	rotation(_rotation),
	scale(_scale)
{
}

void SceneObject::Draw() const
{
	shader->Use();
	glm::mat4 matrix = glm::mat4(scale);
	matrix = glm::translate(matrix, position / scale);
	matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	shader->setMat4("model", matrix);
	model->Draw(shader);
}