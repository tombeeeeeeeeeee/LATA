#pragma once

#include <soloud.h>
#include <soloud_wav.h>

#undef min
#undef max

class Audio
{
public:

	SoLoud::Soloud soloud;
	void Initialise();
	void Deinitialise();

	void GUI();

	void PlaySound(SoLoud::Wav& sound);

	// Busses are like mixers
	SoLoud::Bus musicBus;
	float musicBusVolume = 1.0f;
	int musicBusHandle;

	SoLoud::Wav music;

	SoLoud::Wav eccoDash;
	SoLoud::Wav rainbowShot;

	SoLoud::Wav syncShot;

	SoLoud::Wav enemyHit;
};

