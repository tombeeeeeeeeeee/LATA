#pragma once
#include <string>

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Bollard
{
	Bollard() {};
	Bollard(toml::table table);

	bool startsUp = false;
	bool state = false;
	float timeToRaise = 0.5f;
	float timeToLower = 0.2f;
	std::string triggerTag;

	void GUI();
	toml::table Serialise(unsigned long long guid);
};