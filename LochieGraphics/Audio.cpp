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

	// Set up paths here

	testMixer.Init(soloud);
	musicMixer.Init(soloud);

	//soloud.setVolume(musicMixer.handle, 0.36f);

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
