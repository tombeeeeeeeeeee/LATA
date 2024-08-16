#include "Spotlight.h"

#include "Utilities.h"

#include "imgui.h"

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
	std::string tag = Utilities::PointerToString(this);
	ImGui::DragFloat3(("Position##" + tag).c_str(), &position[0], 0.01f);
	ImGui::DragFloat3(("Direction##" + tag).c_str(), &direction[0], -1.f, 1.f);
	//Tom Changed this, feel free to set it back.
	if (ImGui::DragFloat(("Range##" + tag).c_str(), &range, 0.1f))
		SetRange(range);
	//ImGui::SliderFloat(("Constant##" + tag).c_str(),  &constant,    0.f, 1.f);
	//ImGui::SliderFloat(("Linear##" + tag).c_str(),    &linear,      0.f, 1.f);
	//ImGui::SliderFloat(("Quadratic##" + tag).c_str(), &quadratic,   0.f, 1.f);
	ImGui::SliderFloat(("Cut off##" + tag).c_str(), &cutOff, 0.f, 1.f);
	ImGui::SliderFloat(("Outer cut off##" + tag).c_str(), &outerCutOff, 0.f, 1.f);
}

toml::table Spotlight::Serialise() const
{
	auto table = PointLight::Serialise();
	table.emplace("direction", Serialisation::SaveAsVec3(direction));
	table.emplace("cutOff", cutOff);
	table.emplace("outerCutOff", outerCutOff);
	return table;
}

Light::Type Spotlight::getType() const
{
	return Light::Type::spot;
}

