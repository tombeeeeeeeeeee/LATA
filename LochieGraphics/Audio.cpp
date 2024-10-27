#include "Audio.h"

#include "Paths.h"
#include "Utilities.h"
#include "UserPreferences.h"

#include "EditorGUI.h"

void Audio::Initialise()
{
	soloud.init();
	music.load((Paths::importAudioLocation + "Battle_Theme.wav").c_str());
	music.setLooping(true);
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
