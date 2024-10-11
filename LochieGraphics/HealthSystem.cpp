#include "HealthSystem.h"

#include "Health.h"
#include "ModelRenderer.h"
#include "SceneObject.h"
#include "Material.h"
#include "imgui.h"

#include "Utilities.h"

void HealthSystem::Start(std::unordered_map<unsigned long long, Health>& healths)
{
	for (auto& healthPair : healths)
	{
		healthPair.second.onHealthDown.push_back([this](HealthPacket hp) {OnHealthDown(hp); });
	}
}

void HealthSystem::Update(
	std::unordered_map<unsigned long long, Health>& healths,
	std::unordered_map<unsigned long long, ModelRenderer>& renderers,
	float delta
)
{
	for (auto& healthPair : healths)
	{
		float timeSinceChange = healthPair.second.timeSinceLastChange;
		timeSinceChange /= colourTime;

		if (healthPair.second.lastChangeAmount > 0.0f)
		{
			if (timeSinceChange > 0.0f && timeSinceChange < 1.0f)
			{
				renderers[healthPair.first].tintDelta = 1.0f - timeSinceChange;
				renderers[healthPair.first].alternativeMaterialTint = healColour;
			}
		}
		else
		{
			if (timeSinceChange > 0.0f && timeSinceChange < 1.0f)
			{
				renderers[healthPair.first].tintDelta = 1.0f - timeSinceChange;
				renderers[healthPair.first].alternativeMaterialTint = damageColour;
			}
		}
		
		healthPair.second.timeSinceLastChange += delta;
	}
}

void HealthSystem::PlayerHealing(Health* eccoHealth, Health* syncHealth, glm::vec2 eccoPos, glm::vec2 syncPos, float delta)
{
	
}

void HealthSystem::GUI()
{
	ImGui::ColorEdit3("Health Colour", &healColour[0]);
	ImGui::ColorEdit3("Damage Colour", &damageColour[0]);
	ImGui::DragFloat("Health Colour Time", &colourTime, 1, 0);
	ImGui::DragInt("Heals Per Pulse", &healPerPulse, 1, 0);
	ImGui::DragFloat("CoolDown To On Heal Ability", &cooldown, 1, 0);
	ImGui::DragFloat("TimeBetweenPulses", &timeBetweenPulses, 1, 0);
	ImGui::DragFloat("Time Since Last Heal Ability", &timeSinceLastHealingAbility, 1, 0);
	ImGui::End();
}

void HealthSystem::OnHealthDown(HealthPacket healthPacket)
{

}
