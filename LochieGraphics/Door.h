#pragma once
#include <string>

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Door
{
public:
	bool startClosed = true;
	bool left = false;
	float timeToClose = 0.5f;
	float timeToOpen = 0.5f;
	float timeInMovement = 0.0f;
	std::string	triggerTag;
	bool state = false;
	Door() {};
	Door(toml::table table);

	toml::table Serialise(unsigned long long guid);
	void TriggerCall(std::string tag, bool toggle);
	void GUI();
};