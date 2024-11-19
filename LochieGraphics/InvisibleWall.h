#pragma once
#include "Serialisation.h"

struct ShadowWall
{
	void GUI();
	ShadowWall() {};
	ShadowWall(toml::table table) {};

	toml::table Serialise(unsigned long long guid) { return toml::table{ {"guid", guid} }; };
};