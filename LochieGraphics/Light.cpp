#include "Light.h"

#include "Utilities.h"

#include "imgui.h"

Light::Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular)
{
}

glm::mat4 Light::getShadowViewProjection() const
{
	return getShadowProjection() * getShadowView();
}

void Light::GUI()
{
	std::string tag = PointerToString(this);
	ImGui::ColorEdit3(("Ambient##" + tag).c_str(), &ambient[0]);
	ImGui::ColorEdit3(("Diffuse##" + tag).c_str(), &diffuse[0]);
	ImGui::ColorEdit3(("Specular##" + tag).c_str(), &specular[0]);

	ImGui::Text("Shadows Planes");
	ImGui::DragFloat(("Near plane##" + tag).c_str(), &shadowNearPlane, 0.01f, 0.01f, FLT_MAX);
	ImGui::DragFloat(("Far plane##" + tag).c_str(), &shadowFarPlane, 0.01f, 0.01f, FLT_MAX);

	ImGui::BeginDisabled();
	int shadowWidth = shadowTexWidth;
	int shadowHeight = shadowTexHeight;
	ImGui::DragInt("Shadow Texture Width", &shadowWidth, 1, 0, INT_MAX);
	ImGui::DragInt("Shadow Texture Height", &shadowHeight, 1, 0, INT_MAX);
	// TODO: Set width height
	ImGui::EndDisabled();
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
	std::string tag = PointerToString(this);
	Light::GUI();
	ImGui::DragFloat3(("Direction##" + tag).c_str(), &direction[0], 0.01f, -1.f, 1.f);
	ImGui::DragFloat(("Fake Pos Distance##" + tag).c_str(), &fakePosDistance, 0.01f);
	ImGui::DragFloat(("Projection Width##" + tag).c_str(), &projectionWidth, 0.01f);
}

glm::vec3 DirectionalLight::getPos() const
{
	return -fakePosDistance * direction;
}

glm::mat4 DirectionalLight::getShadowProjection() const
{
	return glm::ortho(-projectionWidth, projectionWidth, -projectionWidth, projectionWidth, shadowNearPlane, shadowFarPlane);
}

glm::mat4 DirectionalLight::getShadowView() const
{
	return glm::lookAt(getPos(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
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
	ImGui::DragFloat("Projection FOV", &projectionFov, 0.1f);
	ImGui::DragFloat3(("Position##" + tag).c_str(),   &position[0], 0.01f);
	ImGui::SliderFloat(("Constant##" + tag).c_str(),  &constant,    0.f, 1.f);
	ImGui::SliderFloat(("Linear##" + tag).c_str(),    &linear,      0.f, 1.f);
	ImGui::SliderFloat(("Quadratic##" + tag).c_str(), &quadratic,   0.f, 1.f);
	ImGui::BeginDisabled();
	ImGui::DragInt(("Index##" + tag).c_str(), &index, 0.01f, 0, 4); //TODO: Replace 4 with the correct number of lights
	ImGui::EndDisabled();
}

glm::vec3 PointLight::getPos() const
{
	return position;
}

glm::mat4 PointLight::getShadowProjection() const
{
	// TODO:
	//return glm::ortho(-projectionWidth, projectionWidth, -projectionWidth, projectionWidth, shadowNearPlane, shadowFarPlane);

	return glm::perspective(glm::radians(projectionFov), (float)shadowTexWidth / (float)shadowTexHeight, shadowNearPlane, shadowFarPlane);
}

glm::mat4 PointLight::getShadowView() const
{
	return glm::lookAt(getPos(), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.0f, 0.f));
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
	ImGui::DragFloat3(("Position##" + tag).c_str(),       &position[0],   0.01f);
	ImGui::DragFloat3(("Direction##" + tag).c_str(),      &direction[0], -1.f, 1.f);
	ImGui::SliderFloat(("Constant##" + tag).c_str(),      &constant,      0.f, 1.f);
	ImGui::SliderFloat(("Linear##" + tag).c_str(),        &linear,        0.f, 1.f);
	ImGui::SliderFloat(("Quadratic##" + tag).c_str(),     &quadratic,     0.f, 1.f);
	ImGui::SliderFloat(("Cut off##" + tag).c_str(),       &cutOff,        0.f, 1.f);
	ImGui::SliderFloat(("Outer cut off##" + tag).c_str(), &outerCutOff,   0.f, 1.f);
}