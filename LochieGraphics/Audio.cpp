#include "Audio.h"

#include <soloud_wav.h>

void Audio::Initialise()
{
	soloud.init();
}

void Audio::Deinitialise()
{
	soloud.deinit();
}
