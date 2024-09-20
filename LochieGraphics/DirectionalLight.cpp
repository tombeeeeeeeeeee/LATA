#include "DirectionalLight.h"

#include "Shader.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

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
	std::string tag = Utilities::PointerToString(this);
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

toml::table DirectionalLight::Serialise() const
{
	auto table = Light::Serialise();
	table.emplace("direction", Serialisation::SaveAsVec3(direction));
	table.emplace("distance", fakePosDistance);

	return table;
}

Light::Type DirectionalLight::getType() const
{
	return Light::Type::directional;
}
