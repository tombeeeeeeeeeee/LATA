#pragma once
#include "Maths.h"

#include <unordered_map>

class Health;
class ModelRenderer;
struct HealthPacket;

class HealthSystem
{
public:
	void Start(
		std::unordered_map<unsigned long long, Health>& healths
	);

	void Update(
		std::unordered_map<unsigned long long, Health>& healths,
		std::unordered_map<unsigned long long, ModelRenderer>& renderers,
		float delta
	);

	void PlayerHealing(
		Health* eccoHealth, Health* syncHealth,
		glm::vec2 eccoPos, glm::vec2 syncPos,
		float delta
	);

	void GUI();

	glm::vec3 damageColour = { 0.8f,0.0f,0.0f };
	glm::vec3 healColour = { 0.0f,0.8f,0.0f };
	float colourTime = 0.1f;
	
	int healPerPulse = 2;
	int pulses = 5;
	float cooldown = 30.0f;
	float timeBetweenPulses = 0.2f;

private:
	bool playerHealingAbility = false;
	int currentPulseCount = 0;
	float timeSinceLastLOS = 0.0f;
	float timeSinceLastPulse = 0.0f;
	float timeSinceLastHealingAbility = 0.0f;

	void OnHealthDown(HealthPacket healthPacket);
};

