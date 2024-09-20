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

