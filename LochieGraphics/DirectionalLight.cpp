#include "DirectionalLight.h"

#include "Shader.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

DirectionalLight::DirectionalLight(glm::vec3 _colour, glm::vec3 _direction) :
	colour(_colour),
	direction(_direction)
{
}


void DirectionalLight::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::ColorEdit3(("Colour##" + tag).c_str(), &colour[0]);
	ImGui::DragFloat3(("Direction##" + tag).c_str(), &direction[0], 0.01f, -1.f, 1.f);
}


toml::table DirectionalLight::Serialise() const
{
	return toml::table{
		{"direction", Serialisation::SaveAsVec3(direction)},
		{"colour", Serialisation::SaveAsVec3(colour)},
	};
}
