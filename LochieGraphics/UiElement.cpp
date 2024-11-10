#include "UiElement.h"

#include "Shader.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

#include <iostream>

void UiElement::setLeftRight(float left, float right)
{
	scale.x = (right - left) / 2.0f;
	offset.x = (left + right) / 2.0f;
}

void UiElement::setDownUp(float down, float up)
{
	scale.y = (up - down) / 2.0f;
	offset.y = (down + up) / 2.0f;
}

void UiElement::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::SliderFloat2(("offset##" + tag).c_str(), &offset.x, -1.0f, 1.0f);
	ImGui::SliderFloat2(("scale##" + tag).c_str(), &scale.x, -1.0f, 1.0f);
	
	float left = -scale.x + offset.x;
	float right = scale.x + offset.x;
	float up = scale.y + offset.y;
	float down = -scale.y + offset.y;

	if (ImGui::SliderFloat(("left##" + tag).c_str(), &left, -1.0f, 1.0f)) {
		setLeftRight(left, right);
	}
	if (ImGui::SliderFloat(("right##" + tag).c_str(), &right, -1.0f, 1.0f)) {
		setLeftRight(left, right);
	}
	if (ImGui::SliderFloat(("bottom##" + tag).c_str(), &down, -1.0f, 1.0f)) {
		setDownUp(down, up);
	}
	if (ImGui::SliderFloat(("up##" + tag).c_str(), &up, -1.0f, 1.0f)) {
		setDownUp(down, up);
	}

	ImGui::BeginDisabled();

	ImGui::Text("For a screen of resolution 1920x1080, the pixel values would be:");
	float width = 1920.0f;
	float height = 1080.0f;
	ImGui::Text(("Left:  " + std::to_string(roundf((left + 1.0f) / 2.0f * width))).c_str());
	ImGui::Text(("Right: " + std::to_string(roundf((right + 1.0f) / 2.0f * width))).c_str());
	ImGui::Text(("Down:  " + std::to_string(roundf((down + 1.0f) / 2.0f * height))).c_str());
	ImGui::Text(("Up:    " + std::to_string(roundf((up + 1.0f) / 2.0f * height))).c_str());
	ImGui::EndDisabled();

	ImGui::ColorEdit3(("backgroundColour##" + tag).c_str(), &backgroundColour.x);
	ImGui::ColorEdit3(("foregroundColour##" + tag).c_str(), &foregroundColour.x);
}

void UiElement::ApplyToShader(Shader* shader, float percent) const
{
	shader->setVec2("offset", offset);
	shader->setVec2("scale", scale);
	shader->setFloat("healthPercent", percent);
	shader->setVec3("backgroundColour", backgroundColour);
	shader->setVec3("healthColour", foregroundColour);

}

toml::table UiElement::Serialise() const
{
	toml::table table{
		{ "offset", Serialisation::SaveAsVec2(offset) },
		{ "scale", Serialisation::SaveAsVec2(scale)},
		{ "backgroundColour", Serialisation::SaveAsVec3(backgroundColour) },
		{ "foregroundColour", Serialisation::SaveAsVec3(foregroundColour) },
	};
	return table;
}

void UiElement::Load(toml::table table)
{
	offset = Serialisation::LoadAsVec2(table["offset"]);
	scale = Serialisation::LoadAsVec2(table["scale"]);
	backgroundColour = Serialisation::LoadAsVec3(table["backgroundColour"], glm::vec3(0.3f, 0.3f, 0.3f));
	foregroundColour = Serialisation::LoadAsVec3(table["foregroundColour"], glm::vec3(0.3f, 1.0f, 0.3f));
}

void UiElement::Load(toml::table* table)
{
	if (!table) {
		std::cout << "Failed to read UiElement";
		return;
	}
	Load(*table);
}

UiElement::UiElement(toml::table table)
{
}
