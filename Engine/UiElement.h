#pragma once

#include "Maths.h"

class Shader;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class UiElement
{
private:
	void setLeftRight(float left, float right);
	void setDownUp(float down, float up);

	glm::vec2 offset = { 0.0f, 0.0f };
	glm::vec2 scale = { 0.5f, 0.5f };
	glm::vec3 backgroundColour{};
	glm::vec3 foregroundColour{};

	bool horizontal = true;

public:
	void GUI();

	void ApplyToShader(Shader* shader, float percent) const;

	toml::table Serialise() const;
	void Load(toml::table table);
	void Load(toml::table* table);

	UiElement(toml::table table);
	UiElement() = default;
};

