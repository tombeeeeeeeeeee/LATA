#include "PointLight.h"

#include "Shader.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

PointLight::PointLight(glm::vec3 _colour, glm::vec3 _position, float _constant, float _linear, float _quadratic, int _index) :
	Light(_colour),
	position(_position),
	constant(1.0f),
	linear(_linear),
	quadratic(_quadratic),
	index(_index)
{
	SetRange(_linear, _quadratic);
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
	std::string tag = Utilities::PointerToString(this);
	ImGui::DragFloat("Projection FOV", &projectionFov, 0.1f);
	ImGui::DragFloat3(("Position##" + tag).c_str(), &position[0], 0.01f);
	//Tom Changed this, feel free to set it back.
	if (ImGui::DragFloat(("Range##" + tag).c_str(), &range, 0.1f))
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

void PointLight::SetRange(float _range)
{
	range = _range;
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

void PointLight::SetRange(float _linear, float _quadratic)
{

	if (_quadratic >= 1.8f)
	{
		range = 7;
	}

	else if (quadratic >= 0.44f)
	{
		float ratio = _quadratic - (0.44f - 1.8f);
		ratio /= -1.8f;
		range = 7 + (13 - 7) * ratio;
	}

	else if (range <= 20)
	{
		float ratio = _quadratic - (0.2f - 0.44f);
		ratio /= -0.44f;
		range = 13 + (20 - 13) * ratio;
	}

	else if (range <= 32)
	{
		float ratio = _quadratic - (0.07f - 0.2f);
		ratio /= -0.2f;
		range = 20 + (32 - 20) * ratio;
	}

	else if (range <= 50)
	{
		float ratio = _quadratic - (0.032f - 0.07f);
		ratio /= -0.07f;
		range = 32 + (50 - 32) * ratio;
	}

	else if (range <= 65)
	{
		float ratio = _quadratic - (0.017f - 0.032f);
		ratio /= -0.032f;
		range = 50 + (65 - 50) * ratio;
	}

	else if (range <= 100)
	{
		float ratio = _quadratic - (0.0075f - 0.017f);
		ratio /= -0.017f;
		range = 65 + (100 - 65) * ratio;
	}

	else if (range <= 160)
	{
		float ratio = _quadratic - (0.0028f - 0.0075f);
		ratio /= -0.0075f;
		range = 100 + (160 - 100) * ratio;
	}
	else if (range <= 200)
	{
		float ratio = _quadratic - (0.0019f - 0.0028f);
		ratio /= -0.0028f;
		range = 160 + (200 - 160) * ratio;
	}
	else if (range <= 325)
	{
		float ratio = _quadratic - (0.0007f - 0.0019f);
		ratio /= -0.0019f;
		range = 200 + (325 - 200) * ratio;
	}
	else if (range <= 600)
	{
		float ratio = _quadratic - (0.0002f - 0.0007f);
		ratio /= -0.0007f;
		range = 325 + (600 - 325) * ratio;
	}
	else
	{
		float ratio = _quadratic - (0.000007f - 0.0002f);
		ratio /= -0.0002f;
		range = 600 + (3250 - 600) * ratio;
	}
}

toml::table PointLight::Serialise() const
{
	auto table = Light::Serialise();
	table.emplace("position", Serialisation::SaveAsVec3(position));
	table.emplace("constant", constant);
	table.emplace("linear", linear);
	table.emplace("quadratic", quadratic);
	table.emplace("range", range);
	table.emplace("index", index);
	table.emplace("projectionFov", projectionFov);
	return table;
}

Light::Type PointLight::getType() const
{
	return Light::Type::point;
}
