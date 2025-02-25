#include "Audio.h"

#include "Paths.h"
#include "Utilities.h"
#include "UserPreferences.h"

#include "EditorGUI.h"

#include <vector>
#include <iostream>

void Audio::CheckForMissingPaths()
{
	std::vector<unsigned int> soundsMissing;
	for (size_t i = 0; i < sounds.size(); i++)
	{
		if (sounds.at(i).filename == "") {
			soundsMissing.push_back((unsigned int)i);
		}
	}
	if (!soundsMissing.empty()) {
		std::cout << "Audio path missing for sounds:";
		for (auto& i : soundsMissing)
		{
			std::cout << " " << i;
		}
		std::cout << '\n';
	}
}

void Audio::Initialise()
{
	soloud.init();

	#pragma region audio filenames
	sounds[railgunMisfire].filename = "";
	sounds[railgunShotFirstCharged].filename = "Railgun_SHOT_NO CHARGE.wav";
	sounds[railgunShotSecondCharged].filename = "Snipe_Shot.wav";
	sounds[railgunFirstChargeReached].filename = "";
	sounds[railgunSecondChargeReached].filename = "Charge_1_Second.wav";
	sounds[railgunHoldMaxCharge].filename = "Railgun_Exhaust.wav";
	sounds[railgunHitEnemy].filename = "Enemy_Shot.wav";
	sounds[syncFootSteps00].filename = "Footsteps_Items_1.wav";
	sounds[syncFootSteps01].filename = "Footsteps_Items_2.wav";
	sounds[syncFootSteps02].filename = "Footsteps_Items_3.wav";
	sounds[syncFootSteps03].filename = "Footsteps_Items_4.wav";
	sounds[syncFootSteps04].filename = "Footsteps_Items_5.wav";
	sounds[syncFootSteps05].filename = "Footsteps_Items_6.wav";
	sounds[syncFootSteps06].filename = "Footsteps_Items_7.wav";
	sounds[syncFootSteps07].filename = "Footsteps_Items_8.wav";
	sounds[syncFootSteps08].filename = "Footsteps_Items_9.wav";
	sounds[syncFootSteps09].filename = "Footsteps_Items_10.wav";
	sounds[syncFootSteps10].filename = "Footsteps_Items_11.wav";
	sounds[syncFootSteps11].filename = "Footsteps_Items_12.wav";
	sounds[syncFootSteps12].filename = "Footsteps_Items_13.wav";
	sounds[syncFootSteps13].filename = "Footsteps_Items_14.wav";
	sounds[syncFootSteps14].filename = "Footsteps_Items_15.wav";
	sounds[syncFootSteps15].filename = "Footsteps_Items_16.wav";
	sounds[syncFootSteps16].filename = "Footsteps_Items_17.wav";
	sounds[syncFootSteps17].filename = "Footsteps_Items_18.wav";
	sounds[syncFootSteps18].filename = "Footsteps_Items_19.wav";
	sounds[syncFootSteps19].filename = "Footsteps_Items_20.wav";
	sounds[syncFootSteps20].filename = "Footsteps_Items_21.wav";
	sounds[syncFootSteps21].filename = "Footsteps_Items_22.wav";
	sounds[syncFootSteps22].filename = "Footsteps_Items_23.wav";
	sounds[syncFootSteps23].filename = "Footsteps_Items_24.wav";
	sounds[syncFootSteps24].filename = "Footsteps_Items_25.wav";
	sounds[syncFootSteps25].filename = "Footsteps_Items_26.wav";
	sounds[syncFootSteps26].filename = "Footsteps_Items_27.wav";
	sounds[syncFootSteps27].filename = "Footsteps_Items_28.wav";
	sounds[syncFootSteps28].filename = "Footsteps_Items_29.wav";
	sounds[syncFootSteps29].filename = "Footsteps_Items_30.wav";
	sounds[syncDeath].filename = "";
	sounds[syncDamageTaken0].filename = "Sync_Damage_1.wav";
	sounds[syncDamageTaken1].filename = "Sync_Damage_2.wav";
	sounds[syncDamageTaken2].filename = "Sync_Damage_3.wav";
	sounds[eccoDrive].filename = "Ecco_Driving_Mk2.wav";
	sounds[eccoBoost].filename = "Ecco_Dash.wav";
	sounds[eccoEnemyHit].filename = "Ecco_Ram_NO_Kill.wav";
	sounds[eccoObstacleHit].filename = "";
	sounds[eccoDeath].filename = "Ecco_Dying.wav";
	sounds[eccoDamageTaken].filename = "Ecco_Shot.wav";
	sounds[rainbowShot].filename = "Ecco_Shot_with_Railgun.wav";
	sounds[rainbowShotMiss].filename = "Ecco_Shot.wav";
	//sounds[enemyDeathRammed].filename = "Ecco_Ram_Enemy_kill.wav";
	sounds[enemyRandomNoise].filename = "";
	sounds[enemyExplode].filename = "Enemy_Killed.wav";
	sounds[enemyRangedShot].filename = "";
	sounds[enemyRangedCharging].filename = "";
	sounds[enemyStunned].filename = "";
	//sounds[enemyDeathShot].filename = "Enemy_Killed.wav";
	sounds[enemyHitByShot].filename = "Enemy_hit_speech.wav";
	sounds[dabMove00].filename = "Slab_Rotate_01.wav";
	sounds[dabMove01].filename = "Slab_Rotate_02.wav";
	sounds[dabMove02].filename = "Slab_Rotate_03.wav";
	sounds[dabMove03].filename = "Slab_Rotate_04.wav";
	sounds[dabMove04].filename = "Slab_Rotate_05.wav";
	sounds[dabMove05].filename = "Slab_Rotate_06.wav";
	sounds[dabMove06].filename = "Slab_Rotate_07.wav";
	sounds[dabMove07].filename = "Slab_Rotate_08.wav";
	sounds[dabMove08].filename = "Slab_Rotate_09.wav";
	sounds[dabMove09].filename = "Slab_Rotate_10.wav";
	sounds[dabMove10].filename = "Slab_Rotate_11.wav";
	sounds[dabMove11].filename = "Slab_Rotate_12.wav";
	sounds[dabMove12].filename = "Slab_Rotate_13.wav";
	sounds[dabMove13].filename = "Slab_Rotate_14.wav";
	sounds[dabMove14].filename = "Slab_Rotate_15.wav";
	sounds[dabMove15].filename = "Slab_Rotate_16.wav";
	sounds[dabMove16].filename = "Slab_Rotate_17.wav";
	sounds[dabMove17].filename = "Slab_Rotate_18.wav";
	sounds[dabMove18].filename = "Slab_Rotate_19.wav";
	sounds[dabMove19].filename = "Slab_Rotate_20.wav";
	sounds[dabMove20].filename = "Slab_Rotate_21.wav";
	sounds[dabMove21].filename = "Slab_Rotate_22.wav";
	sounds[dabMove22].filename = "Slab_Rotate_23.wav";
	sounds[dabMove23].filename = "Slab_Rotate_24.wav";
	sounds[dabMove24].filename = "Slab_Rotate_25.wav";
	sounds[dabMove25].filename = "Slab_Rotate_26.wav";
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

	soloud.setVolume(musicMixer.handle, 0.36f);

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

SoLoud::handle Audio::PlaySound(SoundIndex index, Mixer* mixer)
{
	Sound& sound = sounds[index];
	if (!sound.loaded) { sound.Load(); }
	sounds[index].timesPlayed++;
	if (!mixer) {
		return soloud.play(sounds[index].wav);
	}
	else {
		// TODO: Make sure mixer is initialised
		return mixer->bus.play(sounds[index].wav);
	}
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
