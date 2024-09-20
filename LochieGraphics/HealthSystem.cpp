#include "HealthSystem.h"

#include "Health.h"
#include "ModelRenderer.h"
#include "SceneObject.h"
#include "Material.h"

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
		float timeSinceDamage = healthPair.second.timeSinceLastChange;
		timeSinceDamage /= colourTime;
		//if (timeSinceDamage <= 1.0f)
		{
			timeSinceDamage = glm::clamp(timeSinceDamage, 0.0f, 1.0f);
			std::vector<Material*>& mats = renderers[healthPair.first].materials;
			for (int i = 0; i < mats.size(); i++)
			{
				mats[i]->colour = Utilities::Lerp(damageColour, {1.0f,1.0f,1.0f}, timeSinceDamage);
			}
		}
		healthPair.second.timeSinceLastChange += delta;
	}
}

void HealthSystem::OnHealthDown(HealthPacket healthPacket)
{
	if (healthPacket.so)
	{
		std::vector<Material*>& mats = healthPacket.so->renderer()->materials;
		for (int i = 0; i < mats.size(); i++)
		{
			mats[i]->colour = damageColour;
		}
	}
}
