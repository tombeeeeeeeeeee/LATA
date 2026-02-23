#pragma once

#include <string>

struct Collision;
class SceneObject;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class ExitElevator
{
public:

	ExitElevator() {};
	ExitElevator(toml::table table);

	bool Update(float delta);
	void OnTrigger(Collision collision);

	void GUI(SceneObject* so);

	toml::table Serialise(unsigned long long GUID) const;

	std::string levelToLoad = "";
	void Initialise(SceneObject* so);
private:
	bool hasBeenBound = false;
	bool eccoInExit = false;
	bool syncInExit = false;

	bool countingDown = false;
	float timerTillLevelSwitch = 0.0f;
};

