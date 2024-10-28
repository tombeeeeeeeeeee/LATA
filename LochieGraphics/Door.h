#pragma once
#include <string>
#include "Maths.h"

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
	bool ignoresFalse = false;
	float timeToClose = 0.5f;
	float timeToOpen = 0.5f;
	float timeInMovement = 0.0f;
	float amountToMove = 0.0f;
	std::string	triggerTag;
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	bool state = false;
	Door() {};
	Door(toml::table table);

	toml::table Serialise(unsigned long long guid);
	void TriggerCall(std::string tag, bool toggle);
	void GUI();
};