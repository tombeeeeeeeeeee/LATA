#pragma once

#include <string>

struct Collision;
class SceneObject;
class GameSyncEcco;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class ExitElevator
{
public:

	ExitElevator() {};
	static ExitElevator* Load(toml::table table);
	ExitElevator(toml::table table);

	bool Update(GameSyncEcco* game, float delta);
	void OnTrigger(Collision collision);

	void PartGUI();

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

