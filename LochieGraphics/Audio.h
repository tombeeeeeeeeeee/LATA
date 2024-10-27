#pragma once



#include "soloud.h"

class Audio
{
public:
	SoLoud::Soloud soloud;
	void Initialise();
	void Deinitialise();
};

