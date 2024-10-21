#pragma once
#include <string>

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Bollard
{
public:
	Bollard() {};
	Bollard(toml::table table);

	void TriggerCall(std::string tag, bool toggle);

	bool startsUp = false;
	bool state = false;
	float timeToRaise = 0.5f;
	float timeToLower = 0.2f;
	float timeInProcess = 0.0f;
	std::string triggerTag;

	void GUI();
	toml::table Serialise(unsigned long long guid);
};