#include "Light.h"

#include "Utilities.h"

#include "imguiStuff.h"

Light::Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular)
{
}

void Light::GUI()
{
	std::string tag = PointerToString(this);
	ImGui::DragFloat3(("Ambient##" + tag).c_str(), &ambient[0], 0.01f, 0.f, 1.f);
	ImGui::DragFloat3(("Diffuse##" + tag).c_str(), &diffuse[0], 0.01f, 0.f, 1.f);
	ImGui::DragFloat3(("Specular##" + tag).c_str(), &specular[0], 0.01f, 0.f, 1.f);
}

DirectionalLight::DirectionalLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction) : 
	Light(_ambient, _diffuse, _specular),
	direction(_direction)
{
}

void DirectionalLight::ApplyToShader(Shader* shader)
{
	shader->Use();
	shader->setVec3("directionalLight.direction", direction);
	shader->setVec3("directionalLight.ambient", ambient);
	shader->setVec3("directionalLight.diffuse", diffuse);
	shader->setVec3("directionalLight.specular", specular);
}

void DirectionalLight::GUI()
{
	Light::GUI();
	ImGui::DragFloat3(("Direction##" + PointerToString(this)).c_str(), &direction[0], 0.01f, -1.f, 1.f);
}

PointLight::PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, int _index) :
	Light(_ambient, _diffuse, _specular),
	position(_position),
	constant(_constant),
	linear(_linear),
	quadratic(_quadratic),
	index(_index)
{
}

void PointLight::ApplyToShader(Shader* shader)
{
	shader->Use();
	std::string i = std::to_string(index);
	shader->setVec3("pointLights[" + i + "].position", position);
	shader->setVec3("pointLights[" + i + "].ambient", ambient);
	shader->setVec3("pointLights[" + i + "].diffuse", diffuse);
	shader->setVec3("pointLights[" + i + "].specular", specular);
	shader->setFloat("pointLights[" + i + "].constant", constant);
	shader->setFloat("pointLights[" + i + "].linear", linear);
	shader->setFloat("pointLights[" + i + "].quadratic", quadratic);
}

void PointLight::GUI()
{
	Light::GUI();
	std::string tag = PointerToString(this);
	ImGui::DragFloat3(("Position##" + tag).c_str(), &position[0], 0.01f);
	ImGui::SliderFloat(("Constant##" + tag).c_str(), &constant, 0.f, 1.f);
	ImGui::SliderFloat(("Linear##" + tag).c_str(), &linear, 0.f, 1.f);
	ImGui::SliderFloat(("Quadratic##" + tag).c_str(), &quadratic, 0.f, 1.f);
	ImGui::BeginDisabled();
	ImGui::DragInt(("Index##" + tag).c_str(), &index, 0.01f, 0.f, 1.f);
	ImGui::EndDisabled();
}

Spotlight::Spotlight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff) : PointLight(_ambient, _diffuse, _specular, _position, _constant, _linear, _quadratic, -1),
	direction(_direction), cutOff(_cutOff), outerCutOff(_outerCutOff)
{
}

void Spotlight::ApplyToShader(Shader* shader)
{
	shader->Use();
	shader->setVec3("spotlight.direction", direction);
	shader->setVec3("spotlight.position", position);
	shader->setVec3("spotlight.ambient", ambient);
	shader->setVec3("spotlight.diffuse", diffuse);
	shader->setVec3("spotlight.specular", specular);
	shader->setFloat("spotlight.constant", constant);
	shader->setFloat("spotlight.linear", linear);
	shader->setFloat("spotlight.quadratic", quadratic);
	shader->setFloat("spotlight.cutOff", cutOff);
	shader->setFloat("spotlight.outerCutOff", outerCutOff);
}

void Spotlight::GUI()
{
	Light::GUI();
	std::string tag = PointerToString(this);
	ImGui::DragFloat3(("Position##" + tag).c_str(), &position[0], 0.01f);
	ImGui::DragFloat3(("Direction##" + tag).c_str(), &direction[0], -1.f, 1.f);
	ImGui::DragFloat(("Constant##" + tag).c_str(), &constant, 0.f, 1.f);
	ImGui::SliderFloat(("Linear##" + tag).c_str(), &linear, 0.f, 1.f);
	ImGui::SliderFloat(("Quadratic##" + tag).c_str(), &quadratic, 0.f, 1.f);
	ImGui::SliderFloat(("Cut off##" + tag).c_str(), &cutOff, 0.f, 1.f);
	ImGui::SliderFloat(("Outer cut off##" + tag).c_str(), &outerCutOff, 0.f, 1.f);

}
	