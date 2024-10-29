#include "Audio.h"

#include "Paths.h"
#include "Utilities.h"
#include "UserPreferences.h"

#include "EditorGUI.h"

#include <iostream>

void Audio::Initialise()
{
	soloud.init();
	music.load((Paths::importAudioLocation + "Battle_Theme.wav").c_str());
	music.setLooping(true);
	
	eccoDash.load((Paths::importAudioLocation + "Ecco_Dash.wav").c_str());
	rainbowShot.load((Paths::importAudioLocation + "Ecco_Shot_with_Railgun.wav").c_str());
	syncShot.load((Paths::importAudioLocation + "Snipe_Shot.wav").c_str());
	enemyHit.load((Paths::importAudioLocation + "Enemy_Shot.wav").c_str());
	
	musicBusHandle = soloud.play(musicBus);
	musicBus.play(music);

	
	soloud.setGlobalVolume(UserPreferences::defaultGlobalVolume);
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
	if (ImGui::SliderFloat("Music Mix", &musicBusVolume, 0.0f, 2.0f))
	{
		soloud.setVolume(musicBusHandle, musicBusVolume);
	}
}

void Audio::PlaySound(SoLoud::Wav& sound)
{
	std::cout << "Played a sound!\n";
	soloud.play(sound);
}
