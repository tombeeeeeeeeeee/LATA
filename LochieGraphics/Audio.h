#pragma once

#include <soloud.h>
#include <soloud_wav.h>
#undef min
#undef max

#include <string>
#include <array>

// TODO: Check if using what backend should be

// TODO: Record stats on times sounds have been played to ensure that they aren't missing

class Audio
{
private:


	void CheckForMissingPaths();

public:
	enum SoundIndex : unsigned int {
		COUNT
	};

	class Sound {
	public:
		std::string filename = "";
		bool loaded = false;
		SoLoud::Wav wav;
		bool loop = false;
		void Load();
		unsigned int timesPlayed = 0;
	};

	class Mixer {
	public:
		SoLoud::Bus bus;
		int handle = -1;
		bool initialised = false;
		void Init(SoLoud::Soloud& soloud);
	};

	std::array<Sound, COUNT> sounds;

	SoLoud::Soloud soloud;
	void Initialise();
	void Deinitialise();

	void GUI();

	SoLoud::handle PlaySound(SoundIndex index, Mixer* mixer = nullptr);

	void EnsureAllSoundsLoaded();

	Mixer musicMixer;
	Mixer testMixer;
};

