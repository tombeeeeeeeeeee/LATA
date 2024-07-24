#include "Light.h"

#include "Utilities.h"

#include "imgui.h"

Light::Light(glm::vec3 _colour) :
	colour(_colour)
{
}

glm::mat4 Light::getShadowViewProjection() const
{
	return getShadowProjection() * getShadowView();
}

void Light::GUI()
{
	std::string tag = PointerToString(this);
	//CHECK:
	ImGui::ColorEdit3(("Colour##" + tag).c_str(), &colour[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

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

DirectionalLight::DirectionalLight(glm::vec3 _colour, glm::vec3 _direction) : 
	Light(_colour),
	direction(_direction)
{
}

void DirectionalLight::ApplyToShader(Shader* shader)
{
	shader->Use();
	shader->setVec3("directionalLight.direction", direction);
	shader->setVec3("directionalLight.colour", colour);
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

PointLight::PointLight(glm::vec3 _colour, glm::vec3 _position, float _constant, float _linear, float _quadratic, int _index) :
	Light(_colour),
	position(_position),
	constant(_constant),
	linear(_linear),
	quadratic(_quadratic),
	index(_index)
{
}

PointLight::PointLight(glm::vec3 _colour, glm::vec3 _position, float _range, int _index) :
	Light(_colour),
	position(_position),
	range(_range),
	index(_index)
{
	SetRange(range);
}

void PointLight::ApplyToShader(Shader* shader)
{
	shader->Use();
	std::string i = std::to_string(index);
	shader->setVec3("pointLights[" + i + "].position", position);
	shader->setVec3("pointLights[" + i + "].colour", colour);
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
	//Tom Changed this, feel free to set it back.
	if(ImGui::DragFloat(("Range##" + tag).c_str(), &range, 0.0f))
		SetRange(range);
	//ImGui::SliderFloat(("Constant##" + tag).c_str(),  &constant,    0.f, 1.f);
	//ImGui::SliderFloat(("Linear##" + tag).c_str(),    &linear,      0.f, 1.f);
	//ImGui::SliderFloat(("Quadratic##" + tag).c_str(), &quadratic,   0.f, 1.f);

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

void PointLight::SetRange(float range)
{
	constant = 1.0f;
	if (range <= 7)
	{
		linear = 1 - range * 0.3f / 7;
		quadratic = 1.8f + (7 - range) * 0.5f;
	}

	else if (range <= 13)
	{
		float ratio = range - (13 - 7);
		ratio /= 7;
		linear = 0.7f + (0.35f - 0.7f) * ratio;
		quadratic = 1.8f + (0.44f - 1.8f) * ratio;
	}

	else if (range <= 20)
	{
		float ratio = range - (20 - 13);
		ratio /= 13;
		linear = 0.35f + (0.22f - 0.35f) * ratio;
		quadratic = 0.44f + (0.2f - 0.44f) * ratio;
	}

	else if (range <= 32)
	{
		float ratio = range - (32 - 20);
		ratio /= 20;
		linear = 0.22f + (0.14f - 0.22f) * ratio;
		quadratic = 0.2f + (0.07f - 0.2f) * ratio;
	}

	else if (range <= 50)
	{
		float ratio = range - (50 - 32);
		ratio /= 32;
		linear = 0.14f + (0.09f - 0.14f) * ratio;
		quadratic = 0.07f + (0.032f - 0.07f) * ratio;
	}

	else if (range <= 65)
	{
		float ratio = range - (65 - 50);
		ratio /= 50;
		linear = 0.09f + (0.07f - 0.09f) * ratio;
		quadratic = 0.032f + (0.017f - 0.032f) * ratio;
	}

	else if (range <= 100)
	{
		float ratio = range - (100 - 65);
		ratio /= 65;
		linear = 0.07f + (0.045f - 0.07f) * ratio;
		quadratic = 0.017f + (0.0075f - 0.017f) * ratio;
	}

	else if (range <= 160)
	{
		float ratio = range - (160 - 100);
		ratio /= 100;
		linear = 0.045f + (0.027f - 0.045f) * ratio;
		quadratic = 0.0075f + (0.0028f - 0.0075f) * ratio;
	}
	else if (range <= 200)
	{
		float ratio = range - (200 - 160);
		ratio /= 160;
		linear = 0.027f + (0.022f - 0.027f) * ratio;
		quadratic = 0.0028f + (0.0019f - 0.0028f) * ratio;
	}
	else if (range <= 325)
	{
		float ratio = range - (325 - 200);
		ratio /= 200;
		linear = 0.022f + (0.014f - 0.022f) * ratio;
		quadratic = 0.0019f + (0.0007f - 0.0019f) * ratio;
	}
	else if (range <= 600)
	{
		float ratio = range - (600 - 325);
		ratio /= 325;

		linear = 0.014f + (0.007f - 0.014f) * ratio;
		quadratic = 0.0007f + (0.0002f - 0.0007f) * ratio;
	}
	else
	{
		float ratio = range - (3250 - 600);
		ratio /= 600;
		linear = 0.007f + (0.0014f - 0.007f) * ratio;
		quadratic = 0.0002f + (0.000007f - 0.0002f) * ratio;
	}
}

Spotlight::Spotlight(glm::vec3 _colour, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff) : PointLight(_colour, _position, _constant, _linear, _quadratic, -1),
	direction(_direction), cutOff(_cutOff), outerCutOff(_outerCutOff)
{
}

void Spotlight::ApplyToShader(Shader* shader)
{
	shader->Use();
	shader->setVec3("spotlight.direction", direction);
	shader->setVec3("spotlight.position", position);
	shader->setVec3("spotlight.colour", colour);
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
	//Tom Changed this, feel free to set it back.
	if (ImGui::DragFloat(("Range##" + tag).c_str(), &range, 0.0f))
		SetRange(range);
	//ImGui::SliderFloat(("Constant##" + tag).c_str(),  &constant,    0.f, 1.f);
	//ImGui::SliderFloat(("Linear##" + tag).c_str(),    &linear,      0.f, 1.f);
	//ImGui::SliderFloat(("Quadratic##" + tag).c_str(), &quadratic,   0.f, 1.f);
	ImGui::SliderFloat(("Cut off##" + tag).c_str(),       &cutOff,        0.f, 1.f);
	ImGui::SliderFloat(("Outer cut off##" + tag).c_str(), &outerCutOff,   0.f, 1.f);
}