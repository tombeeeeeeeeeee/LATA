#pragma once
#include <string>
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Triggerable 
{
public:
	std::string triggerTag;

	bool bothPlayersNeeded = true;
	bool falseIsTrue = false;
	bool doesntSendFalseEveryFrame = false;

	void GUI();
	Triggerable() {};
	Triggerable(toml::table table);
	toml::table Serialise(unsigned long long guid);
	void OnTrigger(unsigned int mask);

	bool eccoThisFrame = false;
	bool syncThisFrame = false;
};