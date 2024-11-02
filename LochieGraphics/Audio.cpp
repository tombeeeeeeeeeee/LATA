#include "Audio.h"

#include "Paths.h"
#include "Utilities.h"
#include "UserPreferences.h"

#include "EditorGUI.h"

#include <iostream>

void Audio::CheckForMissingPaths()
{
	for (size_t i = 0; i < sounds.size(); i++)
	{
		if (sounds[i].filename == "") {
			std::cout << "Audio path missing for sound " << i << '\n';
		}
	}
}

void Audio::Initialise()
{
	soloud.init();

	#pragma region audio filenames
	sounds[railgunMisfire].filename = "";
	sounds[railgunShotFirstCharged].filename = "";
	sounds[railgunShotSecondCharged].filename = "Large_Shot.wav";
	sounds[railgunFirstChargeReached].filename = "";
	sounds[railgunSecondChargeReached].filename = "";
	sounds[railgunHoldMaxCharge].filename = "";
	sounds[railgunHitEnemy].filename = "Enemy_Shot.wav";
	sounds[syncFootSteps].filename = "";
	sounds[syncDeath].filename = "";
	sounds[syncDamageTaken].filename = "Sync_Damage_Concept.wav";
	sounds[eccoDrive].filename = "Ecco_Moving_Mk2.wav";
	sounds[eccoBoost].filename = "Ecco_Dash.wav";
	sounds[eccoEnemyHit].filename = "Ecco_Ram_NO_Kill.wav";
	sounds[eccoObstacleHit].filename = "";
	sounds[eccoDeath].filename = "Ecco_Dying.wav";
	sounds[eccoDamageTaken].filename = "";
	sounds[rainbowShot].filename = "Ecco_Shot_with_Railgun.wav";
	sounds[rainbowShotMiss].filename = "Ecco_Shot.wav";
	sounds[enemyDeathRammed].filename = "Ecco_Ram_Enemy_kill.wav";
	sounds[enemyRandomNoise].filename = "";
	sounds[enemyExplode].filename = "Explosion_Close_With_Sound_Effects.wav";
	sounds[enemyRangedShot].filename = "";
	sounds[enemyRangedCharging].filename = "";
	sounds[enemyStunned].filename = "";
	sounds[enemyDeathShot].filename = "Enemy_Killed.wav";
	sounds[enemyHitByShot].filename = "Enemy_hit_speech.wav";
	sounds[bollardMove].filename = "";
	sounds[doorMove].filename = "";
	sounds[pressurePlateActivate].filename = "";
	sounds[healingAbilityActivate].filename = "Heal_Ability_Quick.wav";
	sounds[healingAbilityFailOnCooldown].filename = "Ability_On_cooldown_Alt.wav";
	sounds[healingAbilityFailOutOfRange].filename = "Ability_On_Cooldown.wav";
	sounds[healingAbilityDeactivate].filename = "";
	sounds[uiButtonPress].filename = "";
	sounds[pauseOpen].filename = "";
	sounds[pauseClose].filename = "";
	sounds[uiBack].filename = "";
	sounds[mainMenuMusic].filename = "Battle_Theme.wav";
	sounds[backTrack1].filename = "Battle_Theme_Mk2_Again.wav";
	sounds[backTrack2].filename = "";
#pragma endregion audio filenames
	
	sounds[backTrack1].loop = true;

	testMixer.Init(soloud);
	musicMixer.Init(soloud);
	PlaySound(backTrack1, &musicMixer);

	soloud.setGlobalVolume(UserPreferences::defaultGlobalVolume);

	CheckForMissingPaths();

	std::cout << "Audio Initialised\n";
}

void Audio::Deinitialise()
{
	soloud.deinit();
}

void Audio::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	float globalVolume = soloud.getGlobalVolume();
	if (ImGui::SliderFloat(("Global Volume##" + tag).c_str(), &globalVolume, 0.0f, 2.0f)) {
		soloud.setGlobalVolume(globalVolume);
	}
	float temp = soloud.getVolume(musicMixer.handle);
	if (ImGui::SliderFloat("Music Mix", &temp, 0.0f, 2.0f))
	{
		soloud.setVolume(musicMixer.handle, temp);
	}

	if (ImGui::CollapsingHeader("Test Sounds")) {

		if (ImGui::Button("Stop Playing Any Testing Sounds")) {
			// TODO: maybe be functions via mixer
			soloud.stop(testMixer.handle);
			soloud.play(testMixer.bus);
		}

		ImGui::BeginChild("Sound Test", ImVec2(450, 400), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);

		if (ImGui::BeginTable("Sound List", 5)) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Index");

			ImGui::TableNextColumn();
			ImGui::Text("Path");

			ImGui::TableNextColumn();
			ImGui::Text("Loaded");

			ImGui::TableNextColumn();
			ImGui::Text("Played");

			ImGui::TableNextColumn();
			ImGui::Text("Test");

			for (size_t i = 0; i < COUNT; i++)
			{
				std::string tag = Utilities::PointerToString(&sounds[i]);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(std::to_string(i).c_str());

				ImGui::TableNextColumn();
				ImGui::PushItemWidth(220.0f);
				ImGui::InputText(("##Path" + tag).c_str(), &sounds[i].filename);

				ImGui::TableNextColumn();
				ImGui::BeginDisabled();
				ImGui::Checkbox(("##Loaded" + tag).c_str(), &sounds[i].loaded);
				ImGui::EndDisabled();

				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(sounds[i].timesPlayed).c_str());

				ImGui::TableNextColumn();
				ImGui::PushItemWidth(60.0f);
				if (ImGui::Button(("P##Test" + tag).c_str())) {
					PlaySound((SoundIndex)i, &testMixer);
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();
	}
}

void Audio::PlaySound(SoundIndex index, Mixer* mixer)
{
	Sound& sound = sounds[index];
	if (!sound.loaded) { sound.Load(); }
	if (!mixer) {
		soloud.play(sounds[index].wav);
	}
	else {
		// TODO: Make sure mixer is initialised
		mixer->bus.play(sounds[index].wav);
	}
	sounds[index].timesPlayed++;
}

void Audio::EnsureAllSoundsLoaded()
{
	for (auto& i : sounds) {
		if (!i.loaded) { i.Load(); }
	}
}

void Audio::Sound::Load()
{
	// TODO: MAKE SURE IT ACTUALLY HAS LOADED
	wav.load((Paths::importAudioLocation + filename).c_str());
	loaded = true;
	if (loop) {
		wav.setLooping(true);
	}
	std::cout << "Loaded sound: " << filename << '\n';
}

void Audio::Mixer::Init(SoLoud::Soloud& soloud)
{
	handle = soloud.play(bus);
	initialised = true;
}
