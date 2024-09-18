#pragma once
#include <string>
#include "Serialisation.h"

struct Collision;
class SceneObject;

class ExitElevator
{
public:

	ExitElevator() {};
	ExitElevator(toml::table table);

	void Update();
	void OnTrigger(Collision collision);

	void GUI(SceneObject* so);

	toml::table Serialise(unsigned long long GUID) const;

private:
	void Initialise(SceneObject* so);
	bool hasBeenBound = false;
	bool eccoInExit = false;
	bool syncInExit = false;
	std::string levelToLoad = "";

};

