#pragma once
#include <string>

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class PressurePlate
{
public:
	bool eccoToggled = false;
	bool smallType = false;
	std::string triggerTag = "";
	//represents time in press
	float timeInActuation = 0.0f;
	float actuationAmount = 5.0f;

	bool triggeredThisFrame = false;
	bool triggeredLastFrame = false;
	PressurePlate() {};
	void GUI();

	PressurePlate(toml::table table);
	toml::table Serialise(unsigned long long guid);

	void OnTrigger(int layerMask);
};

