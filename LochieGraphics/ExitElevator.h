#pragma once
#include <string>
struct Collision;
class SceneObject;

class ExitElevator
{
public:

	void Update();
	void OnTrigger(Collision collision);

	void GUI(SceneObject* so);

private:
	void Initialise(SceneObject* so);
	bool hasBeenBound = false;
	bool eccoInExit = false;
	bool syncInExit = false;
	std::string levelToLoad = "";

};

