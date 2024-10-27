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

	// Busses are like mixers
	SoLoud::Bus musicBus;
	float musicBusVolume = 1.0f;
	int musicBusHandle;
	SoLoud::Bus enemyBus;
	SoLoud::Bus syncBus;
	SoLoud::Bus eccoBus;

	SoLoud::Wav music;
};

