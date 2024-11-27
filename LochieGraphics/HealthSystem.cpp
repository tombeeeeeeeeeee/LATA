#include "HealthSystem.h"

#include "Health.h"
#include "ModelRenderer.h"
#include "SceneObject.h"
#include "Material.h"
#include "ExtraEditorGUI.h"
#include "Utilities.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "Collider.h"
#include "Hit.h"
#include "Serialisation.h"
#include "Paths.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Ecco.h"
#include "Sync.h"
#include "SceneObject.h"

#include <iostream>
#include <filesystem>
#include <ostream>
#include <fstream>

void HealthSystem::Load(toml::table table)
{
	damageColour = Serialisation::LoadAsVec3(table["damageColour"]);
	healColour = Serialisation::LoadAsVec3(table["healColour"]);
	colourTime = Serialisation::LoadAsFloat(table["colourTime"]);
	healPerPulse = Serialisation::LoadAsInt(table["healPerPulse"]);
	pulses = Serialisation::LoadAsInt(table["pulses"]);
	healingAbilityCooldown = Serialisation::LoadAsFloat(table["healingAbilityCooldown"]);
	timeBetweenPulses = Serialisation::LoadAsFloat(table["timeBetweenPulses"]);
	healDistance = Serialisation::LoadAsFloat(table["healDistance"]);
	losToleranceTime = Serialisation::LoadAsFloat(table["losToleranceTime"]);
	Health::cooldown = Serialisation::LoadAsFloat(table["cooldown"]);
	abilityUI.Load(table["abilityUI"].as_table());
}

toml::table HealthSystem::Serialise()
{
	return toml::table{
		{ "damageColour", Serialisation::SaveAsVec3(damageColour)},
		{ "healColour", Serialisation::SaveAsVec3(healColour)},
		{ "colourTime", colourTime},
		{ "healPerPulse", healPerPulse},
		{ "pulses", pulses},
		{ "healingAbilityCooldown", healingAbilityCooldown},
		{ "timeBetweenPulses", timeBetweenPulses},
		{ "healDistance", healDistance},
		{ "losToleranceTime", losToleranceTime},
		{ "cooldown", Health::cooldown},
		{ "abilityUI", abilityUI.Serialise() },
	};
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
		PhysicsSystem::RayCast(eccoPos, glm::normalize(syncPos - eccoPos), hits, healDistance, ~(Collider::transparentLayers | (int)CollisionLayers::enemy));
		if (hits.size() > 0 && hits[0].collider->collisionLayer & (int)CollisionLayers::sync)
		{
			playerHealingAbility = true;
			timeSinceLastHealingAbility = 0.0f;
			timeSinceLastLOS = 0.0f;
			timeSinceLastPulse = FLT_MAX;
			currentPulseCount = 0;

			eccoHealthLight = new SceneObject(SceneManager::scene, "eccoLight");
			eccoHealthLight->transform()->setParent(&SceneManager::scene->transforms[SceneManager::scene->ecco->GUID]);
			eccoHealthLight->transform()->setPosition({0.0f,300.0f,0.0f});
			eccoHealthLight->setPointLight(new PointLight(PointLightEffect::On));
			eccoHealthLight->pointLight()->colour = healColour;
			eccoHealthLight->pointLight()->castsShadows = false;
			eccoHealthLight->pointLight()->intensity = 10.0f;
			eccoHealthLight->pointLight()->SetRange(300.0f);

			syncHealthLight = new SceneObject(SceneManager::scene, "syncLight");
			syncHealthLight->transform()->setParent(&SceneManager::scene->transforms[SceneManager::scene->sync->GUID]);
			syncHealthLight->transform()->setPosition({0.0f,300.0f,0.0f});
			syncHealthLight->setPointLight(new PointLight(PointLightEffect::On));
			syncHealthLight->pointLight()->colour = healColour;
			syncHealthLight->pointLight()->castsShadows = false;
			syncHealthLight->pointLight()->intensity = 10.0f;
			syncHealthLight->pointLight()->SetRange(300.0f);
		}
		SceneManager::scene->audio.PlaySound(Audio::healingAbilityActivate);
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
			Particle* syncFX = SceneManager::scene->particleSystem.AddParticle(10, 0.4f, SceneManager::scene->particleSystem.healthParticleTexture, glm::vec3(syncPos.x, 100.0f, syncPos.y));
			syncFX->explodeStrength = 3.0f;
			syncFX->colour = healColour;
			syncFX->Explode();

			Particle* eccoFX = SceneManager::scene->particleSystem.AddParticle(10, 0.4f, SceneManager::scene->particleSystem.healthParticleTexture, glm::vec3(eccoPos.x, 100.0f, eccoPos.y));
			eccoFX->explodeStrength = 3.0f;
			eccoFX->colour = healColour;
			eccoFX->Explode();
		}
		else timeSinceLastPulse += delta;

		std::vector<Hit> hits;
		PhysicsSystem::RayCast(eccoPos, glm::normalize(syncPos - eccoPos), hits, healDistance, ~(Collider::transparentLayers | (int)CollisionLayers::enemy));
		if (hits.size() > 0 && hits[0].collider->collisionLayer & (int)CollisionLayers::sync)
			timeSinceLastLOS = 0.0f;

		else timeSinceLastLOS += delta; 

		//End case;
		if (timeSinceLastLOS > losToleranceTime|| currentPulseCount > pulses)
		{
			playerHealingAbility = false;
			currentPulseCount = 0;
			timeSinceLastHealingAbility = 0.0f;
			SceneManager::scene->DeleteSceneObjectAndChildren(eccoHealthLight->GUID);
			SceneManager::scene->DeleteSceneObjectAndChildren(syncHealthLight->GUID);
			SceneManager::scene->audio.PlaySound(Audio::healingAbilityDeactivate);
		}
	}
	else timeSinceLastHealingAbility += delta;
}


bool HealthSystem::FileSelector(std::string* filename)
{
	std::string* selected = nullptr;
	std::vector<std::string> loadPaths = {};
	std::vector<std::string*> loadPathsPointers = {};
	loadPaths.clear();
	loadPathsPointers.clear();

	for (auto& i : std::filesystem::directory_iterator(Paths::systemPath))
	{
		loadPaths.push_back(i.path().generic_string().substr(Paths::systemPath.size()));
		if (loadPaths.back().substr(loadPaths.back().size() - Paths::healthSystemExtension.size()) != Paths::healthSystemExtension) {
			loadPaths.erase(--loadPaths.end());
			continue;
		}
		loadPaths.back() = loadPaths.back().substr(0, loadPaths.back().size() - Paths::healthSystemExtension.size());
	}
	for (auto& i : loadPaths)
	{
		loadPathsPointers.push_back(&i);
		if (*filename == i) {
			selected = &i;
		}
	}

	if (ExtraEditorGUI::InputSearchBox(loadPathsPointers.begin(), loadPathsPointers.end(), &selected, "Health System Filename", Utilities::PointerToString(filename))) {
		*filename = *selected;
		return true;
	}
	return false;
}

void HealthSystem::GUI()
{
	FileSelector(&filename);

	if (ImGui::Button("Save##HealthSystems")) {

		std::ofstream file(Paths::systemPath + filename + Paths::healthSystemExtension);

		toml::table table = Serialise();

		file << table << '\n';

		file.close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##HealthSystems")) {
		std::ifstream file(Paths::systemPath + filename + Paths::healthSystemExtension);

		toml::table data = toml::parse(file);

		Load(data);

		file.close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save as"))
	{
		saveAs = true;
	}


	SaveAsGUI();
	ImGui::DragFloat("Cooldown for damage", &Health::cooldown, 0.02f, 0.0f);
	ImGui::ColorEdit3("Health Colour", &healColour[0]);
	ImGui::ColorEdit3("Damage Colour", &damageColour[0]);
	ImGui::DragFloat("Health Colour Time", &colourTime, 0.02f, 0);
	ImGui::DragInt("Heals Per Pulse", &healPerPulse, 1, 0);
	ImGui::DragInt("Pulses", &pulses, 1, 0);
	ImGui::DragFloat("CoolDown To On Heal Ability", &healingAbilityCooldown, 0.1f, 0);
	ImGui::DragFloat("TimeBetweenPulses", &timeBetweenPulses, 0.02f, 0);
	ImGui::DragFloat("Time Since Last Heal Ability", &timeSinceLastHealingAbility, 0.02f, 0);
	ImGui::DragFloat("DIstance For Healing Ability", &healDistance, 20.0f, 0);
	ImGui::DragFloat("Tolerance for no Line of Sight", &losToleranceTime, 0.02f, 0);

	if (ImGui::CollapsingHeader("Heal Ability UI")) {
		abilityUI.GUI();
	}

	ImGui::End();
}

void HealthSystem::SaveAsGUI()
{
	if (saveAs)
	{
		ImGui::OpenPopup("Save Health System As");
		saveAs = false;
	}
	if (ImGui::BeginPopupModal("Save Health System As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::InputText("File ", &newFilename);

		if (ImGui::Button("Save"))
		{
			if (newFilename != "") {
				std::ofstream file(Paths::systemPath + newFilename + Paths::healthSystemExtension);
				filename = newFilename;
				toml::table table = Serialise();

				file << table << '\n';

				file.close();
				ImGui::CloseCurrentPopup();

				newFilename = "";
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			newFilename = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
