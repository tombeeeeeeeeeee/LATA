#pragma once

#include "UiElement.h"

#include <unordered_map>
#include <string>

class Health;
class ModelRenderer;
struct HealthPacket;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class HealthSystem
{
public:
	void Load(toml::table table);
	toml::table Serialise();

	void Update(
		std::unordered_map<unsigned long long, Health>& healths,
		std::unordered_map<unsigned long long, ModelRenderer>& renderers,
		float delta
	);

	void PlayerHealingActivate(
		glm::vec2 eccoPos, glm::vec2 syncPos
	);

	void PlayerHealingUpdate(
		Health* eccoHealth, Health* syncHealth,
		glm::vec2 eccoPos, glm::vec2 syncPos,
		float delta
	);

	void GUI();
	void SaveAsGUI();
	glm::vec3 damageColour = { 0.8f,0.0f,0.0f };
	glm::vec3 healColour = { 0.0f,0.8f,0.0f };
	float colourTime = 0.4f;
	
	int healPerPulse = 2;
	int pulses = 5;
	float healingAbilityCooldown = 30.0f;
	float timeBetweenPulses = 0.2f;
	float healDistance = 4000.0f;
	float losToleranceTime = 0.4f;

	std::string filename = "";

	UiElement abilityUI;

	float timeSinceLastHealingAbility = FLT_MAX;
private:
	bool playerHealingAbility = false;
	int currentPulseCount = 0;
	float timeSinceLastLOS = 0.0f;
	float timeSinceLastPulse = 0.0f;

	std::string newFilename = "";
	bool saveAs = false;
};

