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
	bool closed = true;
	bool left = false;
	float timeToClose = 0.5f;
	float timeToOpen = 0.5f;
	float timeInMovement = 0.0f;
	std::string	triggerTag;

	Door() {};
	Door(toml::table table);

	toml::table Serialise(unsigned long long guid);
	void GUI();
};