#include "HealthSystem.h"

#include "Health.h"
#include "ModelRenderer.h"
#include "SceneObject.h"
#include "Material.h"
#include "imgui.h"

#include "Utilities.h"
#include "PhysicsSystem.h"
#include "Collider.h"
#include "Hit.h"

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

void HealthSystem::PlayerHealingActivate(glm::vec2 eccoPos, glm::vec2 syncPos)
{
	if (!playerHealingAbility && timeSinceLastHealingAbility > healingAbilityCooldown)
	{
		std::vector<Hit> hits;
		PhysicsSystem::RayCast(eccoPos, glm::normalize(syncPos - eccoPos), hits, healDistance, Collider::transparentLayers | (int)CollisionLayers::enemy);
		if (hits.size() > 0 && hits[0].collider->collisionLayer & (int)CollisionLayers::sync)
		{
			playerHealingAbility = true;
			timeSinceLastHealingAbility = 0.0f;
			timeSinceLastLOS = 0.0f;
			timeSinceLastPulse = FLT_MAX;
		}
	}
}

void HealthSystem::PlayerHealingUpdate(Health* eccoHealth, Health* syncHealth, glm::vec2 eccoPos, glm::vec2 syncPos, float delta)
{
	if (playerHealingAbility)
	{
		if (timeSinceLastPulse >= timeBetweenPulses)
		{
			eccoHealth->addHealth(healPerPulse);
			syncHealth->addHealth(healPerPulse);
			currentPulseCount++;
			timeSinceLastPulse = 0.0f;
		}
		else timeSinceLastPulse += delta;

		//End case;
		if (timeSinceLastLOS > losToleranceTime|| currentPulseCount > pulses)
		{
			playerHealingAbility = false;
			currentPulseCount = 0;
			timeSinceLastHealingAbility = 0.0f;
		}
	}
	else timeSinceLastHealingAbility += delta;
}

void HealthSystem::GUI()
{
	ImGui::ColorEdit3("Health Colour", &healColour[0]);
	ImGui::ColorEdit3("Damage Colour", &damageColour[0]);
	ImGui::DragFloat("Health Colour Time", &colourTime, 0.02f, 0);
	ImGui::DragInt("Heals Per Pulse", &healPerPulse, 1, 0);
	ImGui::DragFloat("CoolDown To On Heal Ability", &healingAbilityCooldown, 0.1f, 0);
	ImGui::DragFloat("TimeBetweenPulses", &timeBetweenPulses, 0.02f, 0);
	ImGui::DragFloat("Time Since Last Heal Ability", &timeSinceLastHealingAbility, 0.02f, 0);
	ImGui::DragFloat("DIstance For Healing Ability", &healDistance, 20.0f, 0);
	ImGui::DragFloat("Tolerance for no Line of Sight", &losToleranceTime, 0.02f, 0);
	ImGui::End();
}

void HealthSystem::OnHealthDown(HealthPacket healthPacket)
{

}
